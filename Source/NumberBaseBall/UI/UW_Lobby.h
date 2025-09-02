#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Lobby.generated.h"

class UVerticalBox;
class UScrollBox;
class UTextBlock;
class UEditableTextBox;

UCLASS()
class NUMBERBASEBALL_API UUW_Lobby : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UUW_Lobby(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void UpdatePlayerList(const TArray<const FString>& NickNames);
	void UpdateMyNickName(const FString& MyNickName);
	void AddChatMessage(const FText& NewChatMessage);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> UserListBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UScrollBox> ChatScrollBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ChatTextBlock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MyNickNameTextBlock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableTextBox> InputEditableTextBox;

private:
	UFUNCTION()
	void OnCommitChatMessage(const FText& Text, ETextCommit::Type CommitMethod);
};
