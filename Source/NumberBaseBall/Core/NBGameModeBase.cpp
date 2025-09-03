#include "Core/NBGameModeBase.h"
#include "Core/NBGameStateBase.h"
#include "Core/NBGameRoom.h"
#include "Player/Main/NBPlayerController.h"
#include "Player/Main/NBPlayerState.h"

void ANBGameModeBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ANBGameStateBase* NBGameStateBase = GetGameState<ANBGameStateBase>();
	if (IsValid(NBGameStateBase))
	{
		CachedGameState = NBGameStateBase;
	}
	else
	{
		CachedGameState = nullptr;
	}
}

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
	if (IsValid(PlayerController))
	{
		const FString& NickName = PlayerController->GetNickName();
		FString FormatText = FString::Printf(TEXT("%s has joined the game."), *NickName);
		FText Notification = FText::FromString(FormatText);

		SendNotificationToLobby(Notification);
		AddPlayerList(NewPlayer);
	}
}

void ANBGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ANBPlayerController* PlayerController = Cast<ANBPlayerController>(Exiting);
	if (IsValid(PlayerController))
	{
		const FString& NickName = PlayerController->GetNickName();
		FString FormatText = FString::Printf(TEXT("%s has left."), *NickName);
		FText Notification = FText::FromString(FormatText);

		SendNotificationToLobby(Notification);
		RemovePlayerList(Exiting);
	}
}

TArray<TObjectPtr<ANBPlayerController>> ANBGameModeBase::GetPlayersInLobby() const
{
	TArray<TObjectPtr<ANBPlayerController>> InLobby;
	for (ANBPlayerController* PlayerController : PlayerList)
	{
		if (ANBPlayerState* NBPlayerState = PlayerController->GetValidPlayerState<ANBPlayerState>())
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
	if (IsValid(CachedGameState))
	{
		FGameRoom* NewGameRoom = CachedGameState->MakeGameRoom(HostPlayer);
		if (NewGameRoom)
		{
			if (ANBPlayerState* HostPlayerState = HostPlayer->GetValidPlayerState<ANBPlayerState>())
			{
				HostPlayerState->SetPlayerLocationToGameRoom(NewGameRoom->GetRoomId());
				HostPlayer->ClientRPCMakeRoom();
			}
		}
	}
}

void ANBGameModeBase::JoinRoom(ANBPlayerController* GuestPlayer, int32 RoomId)
{
	if (IsValid(CachedGameState))
	{
		FGameRoom* GameRoom = CachedGameState->GetGameRoom(RoomId);
		if (GameRoom && GameRoom->CanJoin())
		{
			GameRoom->JoinGuestInRoom(GuestPlayer);

			if (ANBPlayerState* GuestPlayerState = GuestPlayer->GetValidPlayerState<ANBPlayerState>())
			{
				GuestPlayerState->SetPlayerLocationToGameRoom(RoomId);
				GuestPlayer->ClientRPCMakeRoom();
			}
		}
	}
}

void ANBGameModeBase::LeaveRoom(ANBPlayerController* Exiting)
{
	if (IsValid(CachedGameState))
	{
		FGameRoom* GameRoom = GetGameRoomOfPlayer(Exiting);
		if (GameRoom)
		{
			if (GameRoom->IsHost(Exiting))
			{
				CachedGameState->DestroyGameRoom(GameRoom->GetRoomId());
				LeaveRoom_Host(Exiting, GameRoom->GetGuest());
			}
			else if (GameRoom->IsGuest(Exiting))
			{
				GameRoom->LeaveGuest();
				LeaveRoom_Guest(GameRoom->GetHost(), Exiting);
			}
		}
	}
}

void ANBGameModeBase::GuessNumber(ANBPlayerController* Player, const FText& GuessNumberText)
{
	if (FGameRoom* GameRoom = GetGameRoomOfPlayer(Player))
	{
		int32 RoomId = GameRoom->GetRoomId();
		if (ANBPlayerState* PlayerState = Player->GetValidPlayerState<ANBPlayerState>())
		{
			if (PlayerState->GetPlayerGameState()->HasTurn)
			{
				const FString& NickName = Player->GetNickName();
				const FString& GuessNumberString = GuessNumberText.ToString();

				FString Answer = GameRoomAnwsers[RoomId];
				FString Result = JudgeResult(Answer, GuessNumberText.ToString());

				FString NotificationString = FString::Printf(TEXT("%s guesses %s, result is %s"), *NickName, *GuessNumberString, *Result);
				FText NotificationText = FText::FromString(NotificationString);
				SendNotificationToPlayer(GameRoom->GetHost(), NotificationText);
				SendNotificationToPlayer(GameRoom->GetGuest(), NotificationText);

				if (Result.Equals(FString(TEXT("S3B0"))))
				{
					EndGame(Player, RoomId);
				}
				else
				{
					PlayerState->GetPlayerGameState()->RemainTime = 0.0f;
					OnGameTimerElapsed(RoomId);
				}
			}
			else
			{
				FString NotificationString(TEXT("It's not your turn..."));
				FText NotificationText = FText::FromString(NotificationString);
				SendNotificationToPlayer(Player, NotificationText);
			}
		}
	}

}

void ANBGameModeBase::StartGame(ANBPlayerController* HostPlayer)
{
	if (FGameRoom* GameRoom = GetGameRoomOfPlayer(HostPlayer))
	{
		if (!GameRoom->IsHost(HostPlayer))
		{
			return;
		}

		int32 RoomId = GameRoom->GetRoomId();
		SetRandomNumber(RoomId);
		SetGameRoomTimer(RoomId);

		int32 Dice = FMath::RandRange(0, 1);
		bool IsHostFirst = Dice == 1;

		ANBPlayerState* HostState = GameRoom->GetHostState();
		ANBPlayerState* GuestState = GameRoom->GetGuestState();
		if (IsValid(HostState) && IsValid(GuestState))
		{
			HostState->GetPlayerGameState()->Start();
			GuestState->GetPlayerGameState()->Start();
			if (IsHostFirst)
			{
				HostState->GetPlayerGameState()->GetTurn(TurnDuration);
			}
			else
			{
				GuestState->GetPlayerGameState()->GetTurn(TurnDuration);
			}
		}

		ANBPlayerController* First = IsHostFirst ? GameRoom->GetHost() : GameRoom->GetGuest();
		ANBPlayerController* Next = IsHostFirst ? GameRoom->GetGuest() : GameRoom->GetHost();
		if (IsValid(First) && IsValid(Next))
		{
			SendGameStartNotification(First);
			SendGameStartNotification(Next);
			SendTurnNotification(First, Next);
		}

		GameRoom->StartGame();
		GameSynchronization(RoomId);
	}
}

void ANBGameModeBase::EndGame(ANBPlayerController* Winner, int32 RoomId)
{
	if (FGameRoom* GameRoom = GetGameRoomOfPlayer(Winner))
	{
		ClearGameRoomTimer(RoomId);

		ANBPlayerController* Host = GameRoom->GetHost();
		ANBPlayerController* Guest = GameRoom->GetGuest();
		if (IsValid(Host) && IsValid(Guest))
		{
			SendGameEndNotification(Host, Winner);
			SendGameEndNotification(Guest, Winner);
		}

		GameRoom->EndGame();
		GameSynchronization(RoomId);
	}
}

void ANBGameModeBase::EndGame(int32 RoomId)
{
	if (IsValid(CachedGameState))
	{
		if (FGameRoom* GameRoom = CachedGameState->GetGameRoom(RoomId))
		{
			ClearGameRoomTimer(RoomId);

			ANBPlayerController* Host = GameRoom->GetHost();
			ANBPlayerController* Guest = GameRoom->GetGuest();
			if (IsValid(Host) && IsValid(Guest))
			{
				SendGameEndNotification(Host);
				SendGameEndNotification(Guest);
			}

			GameRoom->EndGame();
			GameSynchronization(RoomId);
		}
	}
}

void ANBGameModeBase::InitPlayerStateUsingOptions(APlayerController* PlayerController, const FString& Options)
{
	ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(PlayerController);
	if (IsValid(NBPlayerController))
	{
		UE_LOG(LogTemp, Log, TEXT("ANBGameModeBase::Login, Options: %s"), *Options);

		if (ANBPlayerState* NBPlayerState = NBPlayerController->GetValidPlayerState<ANBPlayerState>())
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
	if (IsValid(PlayerController))
	{
		FString FormatText = FString::Printf(TEXT("[Game] %s"), *Notification.ToString());
		FText NotificationText = FText::FromString(FormatText);
		PlayerController->ClientRPCReceiveChatMessage(NotificationText);
	}
}

void ANBGameModeBase::SendGameStartNotification(ANBPlayerController* Player)
{
	FString StartString(TEXT("Game Start!"));
	FText StartText = FText::FromString(StartString);
	SendNotificationToPlayer(Player, StartText);
}

void ANBGameModeBase::SendGameEndNotification(ANBPlayerController* Player)
{
	FString StartString(TEXT("Game End!"));
	FText StartText = FText::FromString(StartString);
	SendNotificationToPlayer(Player, StartText);
}

void ANBGameModeBase::SendGameEndNotification(ANBPlayerController* Player, ANBPlayerController* Winner)
{
	const FString& WinnerNickName = Winner->GetNickName();
	FString StartString;
	if (Player == Winner)
	{
		StartString = FString::Printf(TEXT("Game End! You Win!!"), *WinnerNickName);
	}
	else
	{
		StartString = FString::Printf(TEXT("Game End! You Lose. Winner: %s"), *WinnerNickName);
	}
	FText StartText = FText::FromString(StartString);
	SendNotificationToPlayer(Player, StartText);
}

void ANBGameModeBase::SendTurnNotification(ANBPlayerController* CurrentPlayer, ANBPlayerController* NextPlayer)
{
	if (ANBPlayerState* CurrentPlayerState = CurrentPlayer->GetValidPlayerState<ANBPlayerState>())
	{
		FString OtherTurnString(TEXT("Opponent Turn..."));
		FText OtherTurn = FText::FromString(OtherTurnString);

		int32 RemainChance = CurrentPlayerState->GetPlayerGameState()->RemainChance;
		FString YourTurnString = FString::Printf(TEXT("Your Turn!: Remain Chance: %d"), RemainChance);
		FText YourTurn = FText::FromString(YourTurnString);

		SendNotificationToPlayer(CurrentPlayer, YourTurn);
		SendNotificationToPlayer(NextPlayer, OtherTurn);
	}
}

void ANBGameModeBase::LeaveRoom_Host(ANBPlayerController* Host, ANBPlayerController* Guest)
{
	if (IsValid(Host))
	{
		if (ANBPlayerState* HostState = Host->GetValidPlayerState<ANBPlayerState>())
		{
			Host->ClientRPCLeaveRoom();
			HostState->SetPlayerLocationToLobby();
		}
	}
	if (IsValid(Guest))
	{
		if (ANBPlayerState* GuestState = Guest->GetValidPlayerState<ANBPlayerState>())
		{
			Guest->ClientRPCLeaveRoom();
			GuestState->SetPlayerLocationToLobby();
		}
	}
}

void ANBGameModeBase::LeaveRoom_Guest(ANBPlayerController* Host, ANBPlayerController* Guest)
{
	if (IsValid(Host))
	{
		Host->ClientRPCGuestLeavesRoom();
	}
	if (IsValid(Guest))
	{
		if (ANBPlayerState* GuestState = Guest->GetValidPlayerState<ANBPlayerState>())
		{
			Guest->ClientRPCLeaveRoom();
			GuestState->SetPlayerLocationToLobby();
		}
	}
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
	TArray<int32> NumberBox;
	for (int32 N = 1; N <= 9; N++)
	{
		NumberBox.Add(N);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	
	FString Result;
	for (int32 NumCount = 0; NumCount < 3; NumCount++)
	{
		int32 Index = FMath::RandRange(0, NumberBox.Num() - 1);
		Result.Append(FString::FromInt(NumberBox[Index]));
		NumberBox.RemoveAt(Index);
	}
	return Result;
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
	GetWorldTimerManager().SetTimer(GameRoomTimerHandle, TimerDel, TimerInterval, true);
	GameRoomTimers.Add(RoomId, MoveTemp(GameRoomTimerHandle));
}

void ANBGameModeBase::ClearGameRoomTimer(int32 RoomId)
{
	if (GameRoomTimers.Contains(RoomId))
	{
		if (GameRoomTimers[RoomId].IsValid())
		{
			GetWorldTimerManager().ClearTimer(GameRoomTimers[RoomId]);
			GameRoomTimers[RoomId].Invalidate();
		}
	}
}

FString ANBGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0;
	int32 BallCount = 0;

	UE_LOG(LogTemp, Display, TEXT("InSecretNumberString: %s"), *InSecretNumberString);
	UE_LOG(LogTemp, Display, TEXT("InGuessNumberString: %s"), *InGuessNumberString);

	for (int32 A = 0; A < 3; A++)
	{
		for (int32 B = 0; B < 3; B++)
		{
			if (InSecretNumberString[B] == InGuessNumberString[A])
			{
				if (A == B)
				{
					StrikeCount++;
				}
				else
				{
					BallCount++;
				}
			}
		}
	}

	if (StrikeCount + BallCount == 0)
	{
		return TEXT("Out");
	}
	else
	{
		return FString::Printf(TEXT("S%dB%d"), StrikeCount, BallCount);
	}
}

void ANBGameModeBase::OnGameTimerElapsed(int32 RoomId)
{
	if (IsValid(CachedGameState))
	{
		if (FGameRoom* GameRoom = CachedGameState->GetGameRoom(RoomId))
		{
			ANBPlayerController* HasTurnPlayer = nullptr;
			ANBPlayerController* NextPlayer = nullptr;
			ANBPlayerState* HasTurnPlayerState = nullptr;
			ANBPlayerState* NextPlayerState = nullptr;

			if (GameRoom->HostHasTurn())
			{
				HasTurnPlayerState = GameRoom->GetHostState();
				NextPlayerState = GameRoom->GetGuestState();
				HasTurnPlayer = GameRoom->GetHost();
				NextPlayer = GameRoom->GetGuest();
			}
			else if (GameRoom->GuestHasTurn())
			{
				HasTurnPlayerState = GameRoom->GetGuestState();
				NextPlayerState = GameRoom->GetHostState();
				HasTurnPlayer = GameRoom->GetGuest();
				NextPlayer = GameRoom->GetHost();
			}

			if (IsValid(HasTurnPlayerState) && IsValid(NextPlayerState) && IsValid(HasTurnPlayer) && IsValid(NextPlayer))
			{
				if (HasTurnPlayerState->GetPlayerGameState() && NextPlayerState->GetPlayerGameState())
				{
					bool IsTurnEnd = HasTurnPlayerState->GetPlayerGameState()->ReduceTimeAndCheck(TimerInterval);
					if (IsTurnEnd)
					{
						HasTurnPlayerState->GetPlayerGameState()->ReleaseTurn();
						NextPlayerState->GetPlayerGameState()->GetTurn(TurnDuration);

						int32 TotalRemainChange = HasTurnPlayerState->GetPlayerGameState()->RemainChance
							+ NextPlayerState->GetPlayerGameState()->RemainChance;
						bool IsEnd = TotalRemainChange <= 0;
						if (IsEnd)
						{
							EndGame(RoomId);
						}
						else
						{
							SendTurnNotification(NextPlayer, HasTurnPlayer);
						}
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

inline FGameRoom* ANBGameModeBase::GetGameRoomOfPlayer(ANBPlayerController* PlayerController)
{
	if (CachedGameState)
	{
		if (IsValid(PlayerController))
		{
			int32 RoomId = PlayerController->GetRoomId();
			return CachedGameState->GetGameRoom(RoomId);
		}
	}
	return nullptr;
}
