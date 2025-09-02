#include "UI/UW_RoomItem.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

UUW_RoomItem::UUW_RoomItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUW_RoomItem::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(JoinRoomButton))
	{
		bool IsAlreadyBounded = JoinRoomButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnJoinRoomClicked);
		if (!IsAlreadyBounded)
		{
			JoinRoomButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinRoomClicked);
		}
	}
}

void UUW_RoomItem::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(JoinRoomButton))
	{
		bool IsAlreadyBounded = JoinRoomButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnJoinRoomClicked);
		if (IsAlreadyBounded)
		{
			JoinRoomButton->OnClicked.RemoveDynamic(this, &ThisClass::OnJoinRoomClicked);
		}
	}
}

void UUW_RoomItem::OnJoinRoomClicked()
{
	// TODO: 저녁에 여기서부터
	GEngine->AddOnScreenDebugMessage(
		-1, 10, FColor::Cyan, FString::Printf(TEXT("OnJoinRoomClicked: RoomId: %d"), RoomId));
}
