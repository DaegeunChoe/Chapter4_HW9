#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NBGameModeBase.generated.h"

class ANBGameStateBase;
class ANBPlayerController;
struct FGameRoom;

UCLASS()
class NUMBERBASEBALL_API ANBGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void PostInitializeComponents() override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	TArray<TObjectPtr<ANBPlayerController>> GetPlayersInLobby() const;

	void MakeRoom(ANBPlayerController* HostPlayer);
	void JoinRoom(ANBPlayerController* GuestPlayer, int32 RoomId);
	void LeaveRoom(ANBPlayerController* Exiting);
	void GuessNumber(ANBPlayerController* Player, const FText& GuessNumberText);

	void StartGame(ANBPlayerController* HostPlayer);
	void EndGame(ANBPlayerController* Winner, int32 RoomId);
	void EndGame(int32 RoomId);

protected:
	UPROPERTY()
	TArray<TObjectPtr<ANBPlayerController>> PlayerList;

	TMap<int32, FString> GameRoomAnwsers;
	TMap<int32, FTimerHandle> GameRoomTimers;

private:
	void InitPlayerStateUsingOptions(APlayerController* PlayerController, const FString& Options);
	void SendNotificationToLobby(const FText& Notification);
	void SendNotificationToPlayer(ANBPlayerController* PlayerController, const FText& Notification);

	void SendGameStartNotification(ANBPlayerController* Player);
	void SendGameEndNotification(ANBPlayerController* Player);
	void SendGameEndNotification(ANBPlayerController* Player, ANBPlayerController* Winner);
	void SendTurnNotification(ANBPlayerController* CurrentPlayer, ANBPlayerController* NextPlayer);

	void LeaveRoom_Host(ANBPlayerController* Host, ANBPlayerController* Guest);
	void LeaveRoom_Guest(ANBPlayerController* Host, ANBPlayerController* Guest);

	void GameSynchronization(int32 RoomId);

	FString GenerateRandomNumberString();
	void SetRandomNumber(int32 RoomId);
	void SetGameRoomTimer(int32 RoomId);
	void ClearGameRoomTimer(int32 RoomId);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	void OnGameTimerElapsed(int32 RoomId);
	void AddPlayerList(AController* NewPlayer);
	void RemovePlayerList(AController* Exiting);

	FGameRoom* GetGameRoomOfPlayer(ANBPlayerController* PlayerController);

	UPROPERTY(EditDefaultsOnly, Category = "NBGameModeBase|GameConfiguration")
	float TurnDuration = 15.0f;

	UPROPERTY(EditDefaultsOnly, Category = "NBGameModeBase|ServerConfiguration")
	float TimerInterval = 0.5f;

	UPROPERTY()
	TObjectPtr<ANBGameStateBase> CachedGameState;
};
