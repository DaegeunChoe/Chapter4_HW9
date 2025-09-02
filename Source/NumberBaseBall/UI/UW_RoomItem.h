#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_RoomItem.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class NUMBERBASEBALL_API UUW_RoomItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UUW_RoomItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	int32 GetRoomId() const { return RoomId; }
	void SetRoomId(int32 InRoomId) { RoomId = InRoomId; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> RoomNameTextBlock;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> JoinRoomButton;

protected:
	int32 RoomId;

	UFUNCTION()
	void OnJoinRoomClicked();
};
