#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_GameRoom.generated.h"

class UVerticalBox;
class UScrollBox;
class UTextBlock;
class UEditableTextBox;
class UButton;
struct FGameRoom;

UCLASS()
class NUMBERBASEBALL_API UUW_GameRoom : public UUserWidget
{
	GENERATED_BODY()

public:
	UUW_GameRoom(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void InitTextBlocks();
	void UpdateGameRoom(const FGameRoom* GameRoomInfo);
	void AddChatMessage(const FText& NewChatMessage);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UScrollBox> ChatScrollBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ChatTextBlock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> HostTextBlock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> GuestTextBlock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerTextBlock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UEditableTextBox> InputEditableTextBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> GameStartButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> ExitRoomButton;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float CachedRemainTime = 0.0f;

private:
	UFUNCTION()
	void OnCommitChatMessage(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnGameStartButtonClicked();

	UFUNCTION()
	void OnExitRoomButtonClicked();

	void SetTimerText();
	bool IsGuessNumberString(const FString& InNumberString);
};
