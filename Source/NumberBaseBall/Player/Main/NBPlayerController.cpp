#include "Player/Main/NBPlayerController.h"
#include "Player/Main/NBPlayerState.h"
#include "Core/NBGameModeBase.h"
#include "Core/NBGameStateBase.h"
#include "UI/UW_Lobby.h"
#include "UI/UW_GameRoom.h"
#include "UObject/WeakObjectPtrTemplates.h"

FString ANBPlayerController::UnknownNickName(TEXT("Unknown"));

void ANBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		LobbyWidgetInstance = CheckAndCreateWidget<UUW_Lobby>(LobbyWidgetClass);
		GameRoomWidgetInstance = CheckAndCreateWidget<UUW_GameRoom>(GameRoomWidgetClass);
		SwapViewportAndSetInputMode(LobbyWidgetInstance);
	}
}

void ANBPlayerController::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ANBPlayerController::UpdatePlayerList(const TArray<const FString>& NickNames)
{
	if (!HasAuthority())
	{
		if (IsValid(LobbyWidgetInstance))
		{
			LobbyWidgetInstance->UpdatePlayerList(NickNames);
		}
	}
}

void ANBPlayerController::UpdateMyNickName(const FString& MyNickName)
{
	if (!HasAuthority())
	{
		if (IsValid(LobbyWidgetInstance))
		{
			LobbyWidgetInstance->UpdateMyNickName(MyNickName);
		}
	}
}

void ANBPlayerController::UpdateRooms(const TArray<FGameRoom>& GameRooms)
{
	if (!HasAuthority())
	{
		if (IsValid(LobbyWidgetInstance))
		{
			if (LobbyWidgetInstance->IsInViewport())
			{
				LobbyWidgetInstance->UpdateRoomList(GameRooms);
			}
		}
	}
}

void ANBPlayerController::UpdateGameRoomInfo(const FGameRoom* GameRoom)
{
	if (GameRoom)
	{
		if (IsValid(GameRoomWidgetInstance))
		{
			if (GameRoomWidgetInstance->IsInViewport())
			{
				GameRoomWidgetInstance->UpdateGameRoom(GameRoom);
			}
		}
	}
}

const FString& ANBPlayerController::GetNickName()
{
	ANBPlayerState* NBPlayerState = GetPlayerState<ANBPlayerState>();
	if (IsValid(NBPlayerState))
	{
		return NBPlayerState->GetNickName();
	}
	return UnknownNickName;
}

void ANBPlayerController::ServerRPCSendChatMessage_Implementation(const FText& ChatMessage)
{
	if (HasAuthority())
	{
		ANBPlayerState* NBPlayerState = GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPlayerState))
		{
			if (NBPlayerState->GetPlayerLocation() == EPlayerLocation::Lobby)
			{
				BroadcastToLobby(ChatMessage);
			}
			else if (NBPlayerState->GetPlayerLocation() == EPlayerLocation::GameRoom)
			{
				BroadcstToGameRoom(ChatMessage);
			}
		}
	}
}

void ANBPlayerController::ClientRPCReceiveChatMessage_Implementation(const FText& ChatMessage)
{
	if (!HasAuthority() && IsLocalController())
	{
		if (IsValid(LobbyWidgetInstance))
		{
			if (LobbyWidgetInstance->IsInViewport())
			{
				LobbyWidgetInstance->AddChatMessage(ChatMessage);
			}
		}
		if (GameRoomWidgetInstance->IsInViewport())
		{
			if (GameRoomWidgetInstance->IsInViewport())
			{
				GameRoomWidgetInstance->AddChatMessage(ChatMessage);
			}
		}
	}
}

void ANBPlayerController::ServerRPCMakeRoom_Implementation()
{
	if (HasAuthority())
	{
		if (ANBGameModeBase* NBGameModeBase = GetVaildGameMode<ANBGameModeBase>())
		{
			NBGameModeBase->MakeRoom(this);
		}
	}
}

void ANBPlayerController::ServerRPCJoinRoom_Implementation(int32 RoomId)
{
	if (HasAuthority())
	{
		if (ANBGameModeBase* NBGameModeBase = GetVaildGameMode<ANBGameModeBase>())
		{
			NBGameModeBase->JoinRoom(this, RoomId);
		}
	}
}

void ANBPlayerController::ServerRPCLeaveRoom_Implementation()
{
	if (HasAuthority())
	{
		if (ANBGameModeBase* NBGameModeBase = GetVaildGameMode<ANBGameModeBase>())
		{
			NBGameModeBase->LeaveRoom(this);
		}
	}
}

void ANBPlayerController::ServerRPCStartGame_Implementation()
{
	if (HasAuthority())
	{
		if (ANBGameModeBase* NBGameModeBase = GetVaildGameMode<ANBGameModeBase>())
		{
			NBGameModeBase->StartGame(this);
		}
	}
}

void ANBPlayerController::ServerRPCSendGuessNumber_Implementation(const FText& GuessNumberText)
{
	if (HasAuthority())
	{
		if (ANBGameModeBase* NBGameModeBase = GetVaildGameMode<ANBGameModeBase>())
		{
			NBGameModeBase->GuessNumber(this, GuessNumberText);
		}
	}
}

void ANBPlayerController::ClientRPCMakeRoom_Implementation()
{
	if (!HasAuthority() && IsLocalController())
	{
		SwapViewportAndSetInputMode(GameRoomWidgetInstance);
	}
}

void ANBPlayerController::ClientRPCJoinRoom_Implementation(int32 RoomId)
{
	if (!HasAuthority() && IsLocalController())
	{
		SwapViewportAndSetInputMode(GameRoomWidgetInstance);
	}
}

void ANBPlayerController::ClientRPCLeaveRoom_Implementation()
{
	if (!HasAuthority() && IsLocalController())
	{
		SwapViewportAndSetInputMode(LobbyWidgetInstance);
	}
}

void ANBPlayerController::ClientRPCGuestLeavesRoom_Implementation()
{
	if (!HasAuthority())
	{
		ANBPlayerState* NBPlayerState = GetPlayerState<ANBPlayerState>();
		if (IsValid(NBPlayerState))
		{
			NBPlayerState->NotifyToLocalPlayerController();
		}
	}
}

void ANBPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void ANBPlayerController::SwapViewportAndSetInputMode(UUserWidget* TargetWidget)
{
	if (LobbyWidgetInstance->IsInViewport())
	{
		LobbyWidgetInstance->RemoveFromParent();
		LobbyWidgetInstance->InitTextBlocks();
	}
	if (GameRoomWidgetInstance->IsInViewport())
	{
		GameRoomWidgetInstance->RemoveFromParent();
		GameRoomWidgetInstance->InitTextBlocks();
	}

	if (IsValid(TargetWidget))
	{
		TargetWidget->AddToViewport(0);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TargetWidget->GetCachedWidget());
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
}

void ANBPlayerController::BroadcastToLobby(const FText& ChatMessage)
{
	if (HasAuthority())
	{
		if (ANBGameModeBase* NBGameModeBase = GetVaildGameMode<ANBGameModeBase>())
		{
			TArray<TObjectPtr<ANBPlayerController>> PlayerList = NBGameModeBase->GetPlayersInLobby();
			for (ANBPlayerController* PlayerController : PlayerList)
			{
				PlayerController->ClientRPCReceiveChatMessage(ChatMessage);
			}
		}
	}
}

void ANBPlayerController::BroadcstToGameRoom(const FText& ChatMessage)
{
	if (HasAuthority())
	{
		if (ANBGameStateBase* NBGameStateBase = GetVaildGameState<ANBGameStateBase>())
		{
			ANBPlayerState* NBPlayerState = GetPlayerState<ANBPlayerState>();
			if (IsValid(NBPlayerState))
			{
				int32 RoomId = NBPlayerState->GetRoomId();
				FGameRoom* GameRoom = NBGameStateBase->GetGameRoom(RoomId);

				ANBPlayerController* Host = GameRoom->Host;
				ANBPlayerController* Guest = GameRoom->Guest;

				if (IsValid(Host))
				{
					Host->ClientRPCReceiveChatMessage(ChatMessage);
				}
				if (IsValid(Guest))
				{
					Guest->ClientRPCReceiveChatMessage(ChatMessage);
				}
			}
		}
	}
}
