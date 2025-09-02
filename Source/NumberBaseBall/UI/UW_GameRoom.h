#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_GameRoom.generated.h"

class UVerticalBox;
class UScrollBox;
class UTextBlock;
class UEditableTextBox;
class UButton;

UCLASS()
class NUMBERBASEBALL_API UUW_GameRoom : public UUserWidget
{
	GENERATED_BODY()

public:
	UUW_GameRoom(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void AddChatMessage(const FText& NewChatMessage);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UScrollBox> ChatScrollBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ChatTextBlock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MyNickNameTextBlock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableTextBox> InputEditableTextBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ExitRoomButton;

private:
	UFUNCTION()
	void OnCommitChatMessage(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnExitRoomButtonClicked();
};
