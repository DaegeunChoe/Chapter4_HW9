#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NBGameModeBase.generated.h"

class ANBPlayerController;


UCLASS()
class NUMBERBASEBALL_API ANBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	TArray<TObjectPtr<ANBPlayerController>> GetPlayersInLobby() const;

	void MakeRoom(ANBPlayerController* HostPlayer);
	void JoinRoom(ANBPlayerController* GuestPlayer, int32 RoomId);
	void LeaveRoom(ANBPlayerController* Exiting);

	void StartGame(ANBPlayerController* HostPlayer);

protected:
	UPROPERTY()
	TArray<TObjectPtr<ANBPlayerController>> PlayerList;

	TMap<int32, FString> GameRoomAnwsers;
	TMap<int32, FTimerHandle> GameRoomTimers;

private:
	void InitPlayerStateUsingOptions(APlayerController* PlayerController, const FString& Options);
	void SendNotificationToLobby(const FText& Notification);
	void SendNotificationToPlayer(ANBPlayerController* PlayerController, const FText& Notification);

	void GameSynchronization(int32 RoomId);

	FString GenerateRandomNumberString();
	void SetRandomNumber(int32 RoomId);
	void SetGameRoomTimer(int32 RoomId);

	void OnGameTimerElapsed(int32 RoomId);

	void AddPlayerList(AController* NewPlayer);
	void RemovePlayerList(AController* Exiting);

	UPROPERTY(EditDefaultsOnly, Category = "NBGameModeBase|GameConfiguration")
	float TurnDuration = 15.0f;
};
