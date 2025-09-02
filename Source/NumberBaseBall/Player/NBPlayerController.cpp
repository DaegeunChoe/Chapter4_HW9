#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"
#include "Core/NBGameStateBase.h"
#include "UI/UW_Lobby.h"
#include "UI/UW_GameRoom.h"

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

void ANBPlayerController::UpdatePlayerList()
{
	if (HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		ANBGameStateBase* NBGameStateBase = World->GetGameState<ANBGameStateBase>();
		if (IsValid(NBGameStateBase))
		{
			for (auto& OtherPlayerState : NBGameStateBase->PlayerArray)
			{
				ANBPlayerState* NBPlayerState = Cast<ANBPlayerState>(OtherPlayerState);
				if (IsValid(NBPlayerState))
				{
					GEngine->AddOnScreenDebugMessage(-1, 30, FColor::Blue,
						FString::Printf(TEXT("Users: %s"), *NBPlayerState->GetNickName()));
				}
			}
		}
	}
}

void ANBPlayerController::SwapViewportAndSetInputMode(UUserWidget* TargetWidget)
{
	if (LobbyWidgetInstance->IsInViewport())
	{
		LobbyWidgetInstance->RemoveFromViewport();
	}
	if (GameRoomWidgetInstance->IsInViewport())
	{
		GameRoomWidgetInstance->RemoveFromViewport();
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