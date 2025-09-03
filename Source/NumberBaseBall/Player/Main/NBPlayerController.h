#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Main/NBPlayerState.h"
#include "NBPlayerController.generated.h"

class UUW_Lobby;
class UUW_GameRoom;
struct FGameRoom;

UCLASS()
class NUMBERBASEBALL_API ANBPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void UpdatePlayerList(const TArray<const FString>& NickNames);
	void UpdateMyNickName(const FString& MyNickName);
	void UpdateRooms(const TArray<FGameRoom>& GameRooms);
	void UpdateGameRoomInfo(const FGameRoom* GameRoom);

	const FString& GetNickName();
	int32 GetRoomId();

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

	UFUNCTION(Server, Reliable)
	void ServerRPCStartGame();

	UFUNCTION(Server, Reliable)
	void ServerRPCSendGuessNumber(const FText& GuessNumberText);

	UFUNCTION(Client, Reliable)
	void ClientRPCMakeRoom();

	UFUNCTION(Client, Reliable)
	void ClientRPCJoinRoom(int32 RoomId);

	UFUNCTION(Client, Reliable)
	void ClientRPCLeaveRoom();

	UFUNCTION(Client, Reliable)
	void ClientRPCGuestLeavesRoom();

	template <typename T>
	T* GetValidGameMode();

	template <typename T>
	T* GetValidGameState();

	template <typename T>
	T* GetValidPlayerState();

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

	static FString UnknownNickName;
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
inline T* ANBPlayerController::GetValidGameMode()
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(this);
	if (GameMode)
	{
		T* Result = Cast<T>(GameMode);
		if (IsValid(Result))
		{
			return Result;
		}
	}
	return nullptr;
}

template<typename T>
inline T* ANBPlayerController::GetValidGameState()
{
	AGameStateBase* GameState = UGameplayStatics::GetGameState(this);
	if (GameState)
	{
		T* Result = Cast<T>(GameState);
		if (IsValid(Result))
		{
			return Result;
		}
	}
	return nullptr;
}

template<typename T>
inline T* ANBPlayerController::GetValidPlayerState()
{
	if (PlayerState)
	{
		T* Result = Cast<T>(PlayerState);
		if (IsValid(Result))
		{
			return Result;
		}
	}
	return nullptr;
}
