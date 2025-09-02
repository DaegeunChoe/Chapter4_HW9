#include "Core/NBGameModeBase.h"
#include "Core/NBGameStateBase.h"
#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"

APlayerController* ANBGameModeBase::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	SetNickNameFromOptions(NewPlayerController, Options);
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
	// TODO 나중에 로비에 있는 플레이어만 선별해야 함
	return PlayerList;
}


void ANBGameModeBase::SetNickNameFromOptions(APlayerController* PlayerController, const FString& Options)
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
