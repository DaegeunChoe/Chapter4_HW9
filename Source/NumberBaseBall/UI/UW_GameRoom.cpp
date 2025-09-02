#include "UI/UW_GameRoom.h"
#include "Core/NBGameStateBase.h"
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

void UUW_GameRoom::InitTextBlocks()
{
	if (IsValid(ChatTextBlock))
	{
		ChatTextBlock->SetText(FText::FromString(TEXT("[NumberBaseBall GameRoom]")));
	}
	if (IsValid(HostTextBlock))
	{
		HostTextBlock->SetText(FText::FromString(TEXT("Host: ...")));
	}
	if (IsValid(GuestTextBlock))
	{
		GuestTextBlock->SetText(FText::FromString(TEXT("Guest: ...")));
	}
}

void UUW_GameRoom::UpdateGameRoom(const FGameRoom* GameRoomInfo)
{
	if (IsValid(HostTextBlock))
	{
		FString HostNickName;
		if (GameRoomInfo)
		{
			if (IsValid(GameRoomInfo->HostState))
			{
				HostNickName = GameRoomInfo->HostState->GetNickName();
			}
		}
		FString FormatText = FString::Printf(TEXT("Host: %s"), *HostNickName);

		FText HostText = FText::FromString(FormatText);
		HostTextBlock->SetText(HostText);
	}

	if (IsValid(GuestTextBlock))
	{
		FString GuestNickName;
		if (GameRoomInfo)
		{
			if (IsValid(GameRoomInfo->GuestState))
			{
				GuestNickName = GameRoomInfo->GuestState->GetNickName();
			}
		}
		FString FormatText = FString::Printf(TEXT("Guest: %s"), *GuestNickName);

		FText GuestText = FText::FromString(FormatText);
		GuestTextBlock->SetText(GuestText);
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

void UUW_GameRoom::AddChatMessage(const FText& NewChatMessage)
{
	if (IsValid(ChatTextBlock))
	{
		FText OldText = ChatTextBlock->GetText();
		FString FormatString = FString::Printf(TEXT("%s\n%s"), *OldText.ToString(), *NewChatMessage.ToString());
		FText NewText = FText::FromString(FormatString);
		ChatTextBlock->SetText(NewText);
	}

	if (IsValid(ChatScrollBox))
	{
		ChatScrollBox->ScrollToEnd();
	}
}
