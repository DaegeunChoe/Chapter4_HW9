#include "Player/Main/NBPlayerController.h"
#include "Player/Main/NBPlayerState.h"
#include "Core/NBGameModeBase.h"
#include "Core/NBGameStateBase.h"
#include "UI/UW_Lobby.h"
#include "UI/UW_GameRoom.h"
#include "UObject/WeakObjectPtrTemplates.h"

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
		if (ANBPlayerState* NBPlayerState = GetValidPlayerState<ANBPlayerState>())
		{
			NBPlayerState->NotifyToLocalPlayerController();
		}
	}
}
