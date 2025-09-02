#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NBPlayerController.generated.h"

class UUW_Lobby;
class UUW_GameRoom;

UCLASS()
class NUMBERBASEBALL_API ANBPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void UpdatePlayerList(const TArray<const FString>& NickNames);
	void UpdateMyNickName(const FString& MyNickName);

	UFUNCTION(Server, Reliable)
	void ServerRPCSendChatMessage(const FText& ChatMessage);

	UFUNCTION(Client, Reliable)
	void ClientRPCReceiveChatMessage(const FText& ChatMessage);

	UFUNCTION(Server, Reliable)
	void ServerRPCMakeRoom();

	UFUNCTION(Server, Reliable)
	void ServerRPCJoinRoom(int32 RoomId);

	UFUNCTION(Server, Reliable)
	void ServerRPCLeaveRoom();

	UFUNCTION(Client, Reliable)
	void ClientRPCMakeRoom();

	UFUNCTION(Client, Reliable)
	void ClientRPCJoinRoom(int32 RoomId);

	UFUNCTION(Client, Reliable)
	void ClientRPCLeaveRoom();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NBPlayerController|Widget")
	TSubclassOf<UUW_Lobby> LobbyWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NBPlayerController|Widget")
	TSubclassOf<UUW_GameRoom> GameRoomWidgetClass;

	UPROPERTY()
	TObjectPtr<UUW_Lobby> LobbyWidgetInstance;

	UPROPERTY()
	TObjectPtr<UUW_GameRoom> GameRoomWidgetInstance;

private:
	virtual void OnRep_PlayerState() override;

	void SwapViewportAndSetInputMode(UUserWidget* TargetWidget);
	TArray<FString> GetPlayerNickNames();

	void BroadcastToLobby(const FText& ChatMessage);
	void BroadcstToGameRoom(const FText& ChatMessage);

	template <typename T>
	T* CheckAndCreateWidget(TSubclassOf<UUserWidget> WidgetClass);

	template <typename T>
	T* GetGameMode();

	template <typename T>
	T* GetGameState();

	FTimerHandle LatePlayerStateUpdateHandle;
};

template<typename T>
inline T* ANBPlayerController::CheckAndCreateWidget(TSubclassOf<UUserWidget> WidgetClass)
{
	if (IsValid(WidgetClass))
	{
		return CreateWidget<T>(this, WidgetClass);
	}
	return nullptr;
}

template<typename T>
inline T* ANBPlayerController::GetGameMode()
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
	if (GameMode)
	{
		return Cast<T>(GameMode);
	}
	return nullptr;
}

template<typename T>
inline T* ANBPlayerController::GetGameState()
{
	AGameStateBase* GameState = UGameplayStatics::GetGameState(this);
	if (GameState)
	{
		return Cast<T>(GameState);
	}
	return nullptr;
}
