#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"
#include "Core/NBGameModeBase.h"
#include "UI/UW_Lobby.h"
#include "UI/UW_GameRoom.h"
#include "Kismet/GameplayStatics.h"
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

void ANBPlayerController::ServerRPCSendChatMessage_Implementation(const FText& ChatMessage)
{
	if (HasAuthority())
	{
		AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
		if (IsValid(GameMode))
		{
			ANBGameModeBase* NBGameModeBase = Cast<ANBGameModeBase>(GameMode);
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
}

void ANBPlayerController::ClientRPCReceiveChatMessage_Implementation(const FText& ChatMessage)
{
	if (!HasAuthority() && IsLocalController())
	{
		if (IsValid(LobbyWidgetInstance))
		{
			LobbyWidgetInstance->AddChatMessage(ChatMessage);
		}
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