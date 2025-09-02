#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NBPlayerState.generated.h"

UENUM(BlueprintType)
enum class EPlayerLocation : uint8
{
	None,
	Lobby,
	GameRoom,
	Max
};

UCLASS()
class NUMBERBASEBALL_API ANBPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ANBPlayerState();
	virtual void PostInitializeComponents() override;
	virtual void BeginDestroy() override;
	virtual void Destroyed() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void NotifyToLocalPlayerController();

	const FString& GetNickName() const { return NickName; }
	EPlayerLocation GetPlayerLocation() const { return PlayerLocation; }

	void SetNickName(FString InNickName) { NickName = InNickName; }
	void SetPlayerLocation(EPlayerLocation InPlayerLocation) { PlayerLocation = InPlayerLocation; }

protected:
	UPROPERTY(ReplicatedUsing = OnRep_NickName)
	FString NickName;

	UPROPERTY(Replicated)
	EPlayerLocation PlayerLocation;

private:
	UFUNCTION()
	void OnRep_NickName();

	virtual void OnRep_Owner() override;

	TArray<const FString> GetPlayerNickNames();
};
