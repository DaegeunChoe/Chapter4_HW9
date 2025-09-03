#include "UI/UW_RoomItem.h"
#include "Player/NBPlayerController.h"
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

void UUW_RoomItem::EnableJoinButton()
{
	if (IsValid(JoinRoomButton))
	{
		JoinRoomButton->SetIsEnabled(true);
	}
}

void UUW_RoomItem::DisableJoinButton()
{
	if (IsValid(JoinRoomButton))
	{
		JoinRoomButton->SetIsEnabled(false);
	}
}

void UUW_RoomItem::OnJoinRoomClicked()
{
	ANBPlayerController* NBPlayerController = GetOwningPlayer<ANBPlayerController>();
	if (IsValid(NBPlayerController))
	{
		NBPlayerController->ServerRPCJoinRoom(RoomId);
	}
}
