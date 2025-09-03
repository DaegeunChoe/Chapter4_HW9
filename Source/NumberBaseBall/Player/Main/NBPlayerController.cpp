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
	if (ANBPlayerState* NBPlayerState = GetValidPlayerState<ANBPlayerState>())
	{
		return NBPlayerState->GetNickName();
	}
	return UnknownNickName;
}

int32 ANBPlayerController::GetRoomId()
{
	if (ANBPlayerState* NBPlayerState = GetValidPlayerState<ANBPlayerState>())
	{
		return NBPlayerState->GetRoomId();
	}
	return -1;
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
