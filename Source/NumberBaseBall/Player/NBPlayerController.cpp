#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"
#include "Core/NBGameModeBase.h"
#include "Core/NBGameStateBase.h"
#include "UI/UW_Lobby.h"
#include "UI/UW_GameRoom.h"
#include "UObject/WeakObjectPtrTemplates.h"

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

	if (LatePlayerStateUpdateHandle.IsValid())
	{
		LatePlayerStateUpdateHandle.Invalidate();
	}
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

void ANBPlayerController::UpdateRoom(const TArray<FGameRoom>& GameRooms)
{
	if (!HasAuthority())
	{
		if (IsValid(LobbyWidgetInstance))
		{
			LobbyWidgetInstance->UpdateRoomList(GameRooms);
		}
	}
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
		ANBGameModeBase* NBGameModeBase = GetGameMode<ANBGameModeBase>();
		if (IsValid(NBGameModeBase))
		{
			NBGameModeBase->MakeRoom(this);
		}
	}
}

void ANBPlayerController::ServerRPCJoinRoom_Implementation(int32 RoomId)
{
	if (HasAuthority())
	{
		ANBGameModeBase* NBGameModeBase = GetGameMode<ANBGameModeBase>();
		if (IsValid(NBGameModeBase))
		{
		}
	}
}

void ANBPlayerController::ServerRPCLeaveRoom_Implementation()
{
	if (HasAuthority())
	{
		ANBGameModeBase* NBGameModeBase = GetGameMode<ANBGameModeBase>();
		if (IsValid(NBGameModeBase))
		{
			NBGameModeBase->LeaveRoom(this);
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

void ANBPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// TODO: 시간이 남으면 리팩토링..
	GetWorldTimerManager().SetTimer(LatePlayerStateUpdateHandle,
		[WeakThis = TWeakObjectPtr<ANBPlayerController>(this)]()
		{
			if (WeakThis.IsValid())
			{
				ANBPlayerController* NBPlayerController = WeakThis.Get();
				if (IsValid(NBPlayerController))
				{
					ANBPlayerState* NBPlayerState = NBPlayerController->GetPlayerState<ANBPlayerState>();
					if (IsValid(NBPlayerState))
					{
						NBPlayerState->NotifyToLocalPlayerController();
					}
				}
			}
		}, 1.0f, false);
}

void ANBPlayerController::SwapViewportAndSetInputMode(UUserWidget* TargetWidget)
{
	if (LobbyWidgetInstance->IsInViewport())
	{
		LobbyWidgetInstance->RemoveFromParent();
	}
	if (GameRoomWidgetInstance->IsInViewport())
	{
		GameRoomWidgetInstance->RemoveFromParent();
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
		ANBGameModeBase* NBGameModeBase = GetGameMode<ANBGameModeBase>();
		if (IsValid(NBGameModeBase))
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
		ANBGameStateBase* NBGameStateBase = GetGameState<ANBGameStateBase>();
		if (IsValid(NBGameStateBase))
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
