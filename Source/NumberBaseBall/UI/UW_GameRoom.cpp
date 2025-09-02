#include "UI/UW_GameRoom.h"
#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"

UUW_GameRoom::UUW_GameRoom(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUW_GameRoom::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(InputEditableTextBox))
	{
		bool IsAlreadyBounded = InputEditableTextBox->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnCommitChatMessage);
		if (!IsAlreadyBounded)
		{
			InputEditableTextBox->OnTextCommitted.AddDynamic(this, &ThisClass::OnCommitChatMessage);
		}
	}

	if (IsValid(ExitRoomButton))
	{
		bool IsAlreadyBounded = ExitRoomButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnExitRoomButtonClicked);;
		if (!IsAlreadyBounded)
		{
			ExitRoomButton->OnClicked.AddDynamic(this, &ThisClass::OnExitRoomButtonClicked);
		}
	}
}

void UUW_GameRoom::NativeDestruct()
{
	Super::NativeDestruct();

	if (InputEditableTextBox)
	{
		bool IsAlreadyBounded = InputEditableTextBox->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnCommitChatMessage);
		if (IsAlreadyBounded)
		{
			InputEditableTextBox->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnCommitChatMessage);
		}
	}

	if (IsValid(ExitRoomButton))
	{
		bool IsAlreadyBounded = ExitRoomButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnExitRoomButtonClicked);;
		if (IsAlreadyBounded)
		{
			ExitRoomButton->OnClicked.RemoveDynamic(this, &ThisClass::OnExitRoomButtonClicked);
		}
	}
}

void UUW_GameRoom::OnCommitChatMessage(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		ANBPlayerController* NBPlayerController = GetOwningPlayer<ANBPlayerController>();
		if (IsValid(NBPlayerController))
		{
			ANBPlayerState* NBPlayerState = NBPlayerController->GetPlayerState<ANBPlayerState>();
			FString NickName = IsValid(NBPlayerState) ? NBPlayerState->GetNickName() : "NONE";
			FString FormatString = FString::Printf(TEXT("%s: %s"), *NickName, *Text.ToString());
			FText ChatMessage = FText::FromString(FormatString);
			NBPlayerController->ServerRPCSendChatMessage(ChatMessage);
		}
		if (IsValid(InputEditableTextBox))
		{
			InputEditableTextBox->SetText(FText());
		}
	}
}

void UUW_GameRoom::OnExitRoomButtonClicked()
{
	ANBPlayerController* NBPlayerController = GetOwningPlayer<ANBPlayerController>();
	if (IsValid(NBPlayerController))
	{
		NBPlayerController->ServerRPCLeaveRoom();
	}
}

