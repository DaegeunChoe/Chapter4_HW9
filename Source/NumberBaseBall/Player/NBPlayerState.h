#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NBPlayerState.generated.h"


UCLASS()
class NUMBERBASEBALL_API ANBPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void PostInitializeComponents() override;
	virtual void BeginDestroy() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	const FString& GetNickName() const { return NickName; }
	void SetNickName(FString InNickName) { NickName = InNickName; }

protected:
	UPROPERTY(ReplicatedUsing = OnRep_NickName)
	FString NickName;

private:
	void NotifyToLocalPlayerController();
	
	UFUNCTION()
	void OnRep_NickName();
};
