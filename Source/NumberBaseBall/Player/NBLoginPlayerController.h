#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NBLoginPlayerController.generated.h"

class UUW_Login;
class UButton;

UCLASS()
class NUMBERBASEBALL_API ANBLoginPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void JoinServer(const FText& ServerIP, const FText& NickName);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LoginPlayerController|Widget")
	TSubclassOf<UUW_Login> LoginWidgetClass;

	UPROPERTY()
	TObjectPtr<UUW_Login> LoginWidgetInstance;

private:
	UUW_Login* CreateLoginWidget();
	void AddToViewportAndSetInputMode(UUserWidget* TargetWidget);
};
