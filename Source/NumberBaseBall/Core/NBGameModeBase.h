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
	void LeaveRoom(ANBPlayerController* Exiting);

protected:
	UPROPERTY()
	TArray<TObjectPtr<ANBPlayerController>> PlayerList;

private:
	void InitPlayerStateUsingOptions(APlayerController* PlayerController, const FString& Options);

	void SendNotificationToLobby(const FText& Notification);

	void AddPlayerList(AController* NewPlayer);
	void RemovePlayerList(AController* Exiting);
};
