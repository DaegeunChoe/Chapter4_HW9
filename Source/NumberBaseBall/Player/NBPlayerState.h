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

	const FString& GetNickName() const { return NickName; }
	void SetNickName(FString InNickName) { NickName = InNickName; }

protected:
	FString NickName;
};
