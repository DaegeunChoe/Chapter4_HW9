#include "Core/NBGameModeBase.h"
#include "Core/NBGameStateBase.h"
#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"

int32 FGameRoom::NextRoomId = 0;

APlayerController* ANBGameModeBase::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	InitPlayerStateUsingOptions(NewPlayerController, Options);
	return NewPlayerController;
}

void ANBGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	ANBPlayerController* PlayerController = Cast<ANBPlayerController>(NewPlayer);
	ANBPlayerState* NBPlayerState = PlayerController->GetPlayerState<ANBPlayerState>();
	FString NickName = IsValid(NBPlayerState) ? NBPlayerState->GetNickName() : "NONE";
	FString FormatText = FString::Printf(TEXT("%s has joined the game."), *NickName);
	FText Notification = FText::FromString(FormatText);

	SendNotificationToLobby(Notification);
	AddPlayerList(NewPlayer);
}

void ANBGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ANBPlayerController* PlayerController = Cast<ANBPlayerController>(Exiting);
	ANBPlayerState* NBPlayerState = PlayerController->GetPlayerState<ANBPlayerState>();
	FString NickName = IsValid(NBPlayerState) ? NBPlayerState->GetNickName() : "NONE";
	FString FormatText = FString::Printf(TEXT("%s has left."), *NickName);
	FText Notification = FText::FromString(FormatText);

	SendNotificationToLobby(Notification);
	RemovePlayerList(Exiting);
}

TArray<TObjectPtr<ANBPlayerController>> ANBGameModeBase::GetPlayersInLobby() const
{
	TArray<TObjectPtr<ANBPlayerController>> InLobby;
	for (ANBPlayerController* PlayerController : PlayerList)
	{
		ANBPlayerState* NBPlayerState = PlayerController->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPlayerState))
		{
			if (NBPlayerState->GetPlayerLocation() == EPlayerLocation::Lobby)
			{
				InLobby.Add(PlayerController);
			}
		}
	}
	return InLobby;
}

void ANBGameModeBase::MakeRoom(ANBPlayerController* HostPlayer)
{
	ANBGameStateBase* NBGameStateBase = GetGameState<ANBGameStateBase>();
	if (IsValid(NBGameStateBase))
	{
		ANBPlayerState* NBPlayerState = HostPlayer->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPlayerState))
		{
			int32 NewRoomId = NBGameStateBase->MakeGameRoom(HostPlayer);
			if (NewRoomId != -1)
			{
				NBPlayerState->SetPlayerLocationToGameRoom(NewRoomId);
				HostPlayer->ClientRPCMakeRoom();
			}
		}
	}
}

void ANBGameModeBase::JoinRoom(ANBPlayerController* GuestPlayer, int32 RoomId)
{
	ANBGameStateBase* NBGameStateBase = GetGameState<ANBGameStateBase>();
	if (IsValid(NBGameStateBase))
	{
		ANBPlayerState* NBPlayerState = GuestPlayer->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPlayerState))
		{
			bool IsSuccess = NBGameStateBase->JoinGameRoom(GuestPlayer, RoomId);
			if (IsSuccess)
			{
				NBPlayerState->SetPlayerLocationToGameRoom(RoomId);
				GuestPlayer->ClientRPCMakeRoom();
			}
		}
	}
}

void ANBGameModeBase::LeaveRoom(ANBPlayerController* Exiting)
{
	ANBGameStateBase* NBGameStateBase = GetGameState<ANBGameStateBase>();
	if (IsValid(NBGameStateBase))
	{
		ANBPlayerState* NBPlayerState = Exiting->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPlayerState))
		{
			// TODO: 리팩토링할 시간이 있다면..
			int32 RoomId = NBPlayerState->GetRoomId();
			if (RoomId != -1)
			{
				FGameRoom* GameRoom = NBGameStateBase->GetGameRoom(RoomId);
				if (GameRoom)
				{
					if (GameRoom->Host == Exiting)
					{
						NBGameStateBase->DestroyGameRoom(RoomId);
						if (IsValid(GameRoom->Guest))
						{
							ANBPlayerState* GuestPlayerState = GameRoom->Guest->GetPlayerState<ANBPlayerState>();
							GuestPlayerState->SetPlayerLocationToLobby();
							GameRoom->Guest->ClientRPCLeaveRoom();
						}
					}
					else if (GameRoom->Guest == Exiting)
					{
						GameRoom->Guest = nullptr;
						GameRoom->GuestState = nullptr;
						if (IsValid(GameRoom->Host))
						{
							GameRoom->Host->ClientRPCGuestLeavesRoom();
						}
					}
				}
			}
			NBPlayerState->SetPlayerLocationToLobby();
			Exiting->ClientRPCLeaveRoom();
		}
	}
}

void ANBGameModeBase::StartGame(ANBPlayerController* HostPlayer)
{
	if (!IsValid(HostPlayer))
	{
		return;
	}

	ANBGameStateBase* NBGameStateBase = GetGameState<ANBGameStateBase>();
	if (IsValid(NBGameStateBase))
	{
		ANBPlayerState* HostPlayerState = HostPlayer->GetPlayerState<ANBPlayerState>();
		if (IsValid(HostPlayerState))
		{
			int32 RoomId = HostPlayerState->GetRoomId();
			if (RoomId != -1)
			{
				FGameRoom* GameRoom = NBGameStateBase->GetGameRoom(RoomId);

				if (GameRoom->Host != HostPlayer)
				{
					return;
				}

				SetRandomNumber(GameRoom->RoomId);
				SetGameRoomTimer(GameRoom->RoomId);

				int32 Dice = FMath::RandRange(0, 1);
				
				if (IsValid(GameRoom->HostState) && (IsValid(GameRoom->GuestState)))
				{
					GameRoom->HostState->GetPlayerGameState()->Start();
					GameRoom->GuestState->GetPlayerGameState()->Start();
					if (Dice == 1)
					{
						GameRoom->HostState->GetPlayerGameState()->GetTurn(TurnDuration);
					}
					else
					{
						GameRoom->GuestState->GetPlayerGameState()->GetTurn(TurnDuration);
					}
				}

				if (IsValid(GameRoom->Host) && IsValid(GameRoom->Guest))
				{
					FString StartString(TEXT("Game Start!"));
					FText StartText = FText::FromString(StartString);
					SendNotificationToPlayer(GameRoom->Host, StartText);
					SendNotificationToPlayer(GameRoom->Guest, StartText);

					FString OtherTurnString(TEXT("Opponent Turn..."));
					FText OtherTurn = FText::FromString(OtherTurnString);
					if (Dice == 1)
					{
						int32 RemainChance = GameRoom->HostState->GetPlayerGameState()->RemainChance;
						FString YourTurnString = FString::Printf(TEXT("Your Turn!: Remain Chance: %d"), RemainChance);
						FText YourTurn = FText::FromString(YourTurnString);
						SendNotificationToPlayer(GameRoom->Host, YourTurn);
						SendNotificationToPlayer(GameRoom->Guest, OtherTurn);
					}
					else
					{
						int32 RemainChance = GameRoom->GuestState->GetPlayerGameState()->RemainChance;
						FString YourTurnString = FString::Printf(TEXT("Your Turn!: Remain Chance: %d"), RemainChance);
						FText YourTurn = FText::FromString(YourTurnString);
						SendNotificationToPlayer(GameRoom->Host, OtherTurn);
						SendNotificationToPlayer(GameRoom->Guest, YourTurn);
					}
				}

				if (GameRoom)
				{
					GameRoom->IsPlaying = true;
				}

				GameSynchronization(GameRoom->RoomId);
			}
		}
	}
}

void ANBGameModeBase::InitPlayerStateUsingOptions(APlayerController* PlayerController, const FString& Options)
{
	ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(PlayerController);
	if (IsValid(NBPlayerController))
	{
		UE_LOG(LogTemp, Log, TEXT("ANBGameModeBase::Login, Options: %s"), *Options);

		ANBPlayerState* NBPlayerState = NBPlayerController->GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPlayerState))
		{
			FString ParseOption;
			FString NickName;
			FParse::Value(*Options, TEXT("nickname="), ParseOption);
			ParseOption.Split("?", &NickName, nullptr);
			UE_LOG(LogTemp, Log, TEXT("ANBGameModeBase::SetNickNameFromOptions, NickName: %s"), *NickName);
			NBPlayerState->SetNickName(NickName);

			NBPlayerState->SetPlayerLocationToLobby();
		}
	}
}

void ANBGameModeBase::SendNotificationToLobby(const FText& Notification)
{
	FString FormatText = FString::Printf(TEXT("[Notification] %s"), *Notification.ToString());
	FText NotificationText = FText::FromString(FormatText);
	for (ANBPlayerController* Player : GetPlayersInLobby())
	{
		Player->ClientRPCReceiveChatMessage(NotificationText);
	}
}

void ANBGameModeBase::SendNotificationToPlayer(ANBPlayerController* PlayerController, const FText& Notification)
{
	FString FormatText = FString::Printf(TEXT("[Game] %s"), *Notification.ToString());
	FText NotificationText = FText::FromString(FormatText);
	PlayerController->ClientRPCReceiveChatMessage(NotificationText);
}

void ANBGameModeBase::GameSynchronization(int32 RoomId)
{
	ANBGameStateBase* NBGameStateBase = GetGameState<ANBGameStateBase>();
	if (IsValid(NBGameStateBase))
	{
		FGameRoom* GameRoom = NBGameStateBase->GetGameRoom(RoomId);
		if (GameRoom)
		{
		}
	}
}

FString ANBGameModeBase::GenerateRandomNumberString()
{
	return FString();
}

void ANBGameModeBase::SetRandomNumber(int32 RoomId)
{
	FString RandomNumberString = GenerateRandomNumberString();
	GameRoomAnwsers.Add(RoomId, RandomNumberString);
}

void ANBGameModeBase::SetGameRoomTimer(int32 RoomId)
{
	FTimerDelegate TimerDel;
	FTimerHandle GameRoomTimerHandle;
	TimerDel.BindUObject(this, &ThisClass::OnGameTimerElapsed, RoomId);
	GetWorldTimerManager().SetTimer(GameRoomTimerHandle, TimerDel, 1.0f, true);
	GameRoomTimers.Add(RoomId, MoveTemp(GameRoomTimerHandle));
}

void ANBGameModeBase::OnGameTimerElapsed(int32 RoomId)
{
	ANBGameStateBase* NBGameStateBase = GetGameState<ANBGameStateBase>();
	if (IsValid(NBGameStateBase))
	{
		FGameRoom* GameRoom = NBGameStateBase->GetGameRoom(RoomId);
		if (GameRoom)
		{
			ANBPlayerController* HasTurnPlayer = nullptr;
			ANBPlayerController* NextPlayer = nullptr;
			ANBPlayerState* HasTurnPlayerState = nullptr;
			ANBPlayerState* NextPlayerState = nullptr;
			
			if (GameRoom->HostState && GameRoom->HostState->GetPlayerGameState() && GameRoom->HostState->GetPlayerGameState()->HasTurn)
			{
				HasTurnPlayerState = GameRoom->HostState;
				NextPlayerState = GameRoom->GuestState;
				HasTurnPlayer = GameRoom->Host;
				NextPlayer = GameRoom->Guest;
			}
			else if (GameRoom->GuestState && GameRoom->GuestState->GetPlayerGameState() && GameRoom->GuestState->GetPlayerGameState()->HasTurn)
			{
				HasTurnPlayerState = GameRoom->GuestState;
				NextPlayerState = GameRoom->HostState;
				HasTurnPlayer = GameRoom->Guest;
				NextPlayer = GameRoom->Host;
			}

			if (IsValid(HasTurnPlayerState) && IsValid(NextPlayerState))
			{
				if (HasTurnPlayerState->GetPlayerGameState() && NextPlayerState->GetPlayerGameState())
				{
					bool IsTurnEnd = HasTurnPlayerState->GetPlayerGameState()->ReduceTimeAndCheck(1.0f);
					if (IsTurnEnd)
					{
						HasTurnPlayerState->GetPlayerGameState()->ReleaseTurn();
						NextPlayerState->GetPlayerGameState()->GetTurn(TurnDuration);

						int32 RemainChance = GameRoom->HostState->GetPlayerGameState()->RemainChance;
						FString YourTurnString = FString::Printf(TEXT("Your Turn!: Remain Chance: %d"), RemainChance);
						FText YourTurn = FText::FromString(YourTurnString);
						FString OtherTurnString(TEXT("Opponent Turn..."));
						FText OtherTurn = FText::FromString(OtherTurnString);

						SendNotificationToPlayer(NextPlayer, YourTurn);
						SendNotificationToPlayer(HasTurnPlayer, OtherTurn);
					}
					GameSynchronization(RoomId);
				}
			}
		}
	}
}

void ANBGameModeBase::AddPlayerList(AController* NewPlayer)
{
	ANBPlayerController* PlayerController = Cast<ANBPlayerController>(NewPlayer);
	if (IsValid(PlayerController))
	{
		if (!PlayerList.Contains(PlayerController))
		{
			PlayerList.Add(PlayerController);
		}
	}
}

void ANBGameModeBase::RemovePlayerList(AController* Exiting)
{
	ANBPlayerController* PlayerController = Cast<ANBPlayerController>(Exiting);
	if (IsValid(PlayerController))
	{
		if (PlayerList.Contains(PlayerController))
		{
			PlayerList.Remove(PlayerController);
		}
	}
}
