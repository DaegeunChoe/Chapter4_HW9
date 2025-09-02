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
					}
				}
			}
			NBPlayerState->SetPlayerLocationToLobby();
			Exiting->ClientRPCLeaveRoom();
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
