#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NBPlayerController.generated.h"

class UUW_Lobby;
class UUW_GameRoom;

UCLASS()
class NUMBERBASEBALL_API ANBPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void UpdatePlayerList();

	UFUNCTION(Server, Reliable)
	void ServerRPCSendChatMessage(const FText& ChatMessage);

	UFUNCTION(Client, Reliable)
	void ClientRPCReceiveChatMessage(const FText& ChatMessage);

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
	void SwapViewportAndSetInputMode(UUserWidget* TargetWidget);
	TArray<FString> GetPlayerNickNames();

	template <typename T>
	T* CheckAndCreateWidget(TSubclassOf<UUserWidget> WidgetClass);
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
