#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"
#include "Core/NBGameStateBase.h"
#include "Core/NBGameModeBase.h"
#include "UI/UW_Lobby.h"
#include "UI/UW_GameRoom.h"
#include "Kismet/GameplayStatics.h"

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
	if (!HasAuthority())
	{
		TArray<FString> NickNames = GetPlayerNickNames();
		if (IsValid(LobbyWidgetInstance))
		{
			LobbyWidgetInstance->UpdatePlayerList(NickNames);
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

TArray<FString> ANBPlayerController::GetPlayerNickNames()
{
	TArray<FString> NickNameArray;

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
					NickNameArray.Emplace(NBPlayerState->GetNickName());
				}
			}
		}
	}
	NickNameArray.Sort();
	return NickNameArray;
}
