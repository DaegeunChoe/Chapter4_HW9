#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Login.generated.h"

class UEditableTextBox;
class UButton;

UCLASS()
class NUMBERBASEBALL_API UUW_Login : public UUserWidget
{
	GENERATED_BODY()

public:
	UUW_Login(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void NativeConstruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ServerIPEditableTextBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableTextBox> NickNameEditableTextBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> LoginButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;

private:
	UFUNCTION()
	void OnLoginButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();
};
