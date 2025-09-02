#include "UI/UW_Lobby.h"
#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Blueprint/WidgetTree.h"

UUW_Lobby::UUW_Lobby(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUW_Lobby::NativeConstruct()
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

	if (IsValid(MakeRoomButton))
	{
		MakeRoomButton->OnClicked.AddDynamic(this, &ThisClass::OnMakeRoomClicked);
	}
}

void UUW_Lobby::NativeDestruct()
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
}

void UUW_Lobby::UpdatePlayerList(const TArray<const FString>& NickNames)
{
	if (IsValid(UserListBox))
	{
		UserListBox->ClearChildren();
		for (const FString& NickName : NickNames)
		{
			UTextBlock* TextBlockWidget = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
			TextBlockWidget->SetText(FText::FromString(NickName));
			UserListBox->AddChildToVerticalBox(TextBlockWidget);
		}
	}
}

void UUW_Lobby::UpdateMyNickName(const FString& MyNickName)
{
	if (IsValid(MyNickNameTextBlock))
	{
		MyNickNameTextBlock->SetText(FText::FromString(MyNickName));
	}
}


void UUW_Lobby::OnCommitChatMessage(const FText& Text, ETextCommit::Type CommitMethod)
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

void UUW_Lobby::OnMakeRoomClicked()
{
	ANBPlayerController* NBPlayerController = GetOwningPlayer<ANBPlayerController>();
	if (IsValid(NBPlayerController))
	{
		NBPlayerController->ServerRPCMakeRoom();
	}
}

void UUW_Lobby::AddChatMessage(const FText& NewChatMessage)
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

