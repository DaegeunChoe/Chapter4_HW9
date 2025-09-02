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

protected:
	UPROPERTY()
	TArray<TObjectPtr<ANBPlayerController>> PlayerList;

private:
	void SetNickNameFromOptions(APlayerController* PlayerController, const FString& Options);
};
