#include "UI/UW_Lobby.h"
#include "UI/UW_RoomItem.h"
#include "Core/NBGameStateBase.h"
#include "Player/Main/NBPlayerController.h"
#include "Player/Main/NBPlayerState.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
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
		bool IsAlreadyBounded = MakeRoomButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnMakeRoomClicked);
		if (!IsAlreadyBounded)
		{
			MakeRoomButton->OnClicked.AddDynamic(this, &ThisClass::OnMakeRoomClicked);
		}
	}
}

void UUW_Lobby::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(InputEditableTextBox))
	{
		bool IsAlreadyBounded = InputEditableTextBox->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnCommitChatMessage);
		if (IsAlreadyBounded)
		{
			InputEditableTextBox->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnCommitChatMessage);
		}
	}

	if (IsValid(MakeRoomButton))
	{
		bool IsAlreadyBounded = MakeRoomButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnMakeRoomClicked);
		if (IsAlreadyBounded)
		{
			MakeRoomButton->OnClicked.RemoveDynamic(this, &ThisClass::OnMakeRoomClicked);
		}
	}
}

void UUW_Lobby::InitTextBlocks()
{
	if (IsValid(ChatTextBlock))
	{
		ChatTextBlock->SetText(FText::FromString(TEXT("[NumberBaseBall Lobby]")));
	}
	if (IsValid(MyNickNameTextBlock))
	{
		MyNickNameTextBlock->SetText(FText::FromString(TEXT("(Connecting...)")));
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

void UUW_Lobby::UpdateRoomList(const TArray<FGameRoom>& GameRooms)
{
	if (IsValid(RoomListPanel))
	{
		RoomListPanel->ClearChildren();

		for (int32 index = 0; index < GameRooms.Num(); index++)
		{
			const FGameRoom& GameRoom = GameRooms[index];
			int32 Row = index / 2;
			int32 Col = index % 2;
			UUW_RoomItem* NewItem = CreateWidget<UUW_RoomItem>(GetOwningPlayer(), RoomItemWidgetClass);
			NewItem->SetRoomId(GameRoom.RoomId);
			NewItem->RoomNameTextBlock->SetText(GameRoom.RoomName);

			bool IsFullGameRoom = (GameRoom.HostState != nullptr) && (GameRoom.GuestState != nullptr);
			bool IsPlayingGameRoom = GameRoom.IsPlaying;
			bool CanJoin = !IsFullGameRoom && !IsPlayingGameRoom;
			if (CanJoin)
			{
				NewItem->EnableJoinButton();
			}
			else
			{
				NewItem->DisableJoinButton();
			}

			UUniformGridSlot* NewSlot = RoomListPanel->AddChildToUniformGrid(NewItem, Row, Col);
			NewSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			NewSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		}
		if (GameRooms.Num() == 1)
		{
			UUW_RoomItem* NewItem = CreateWidget<UUW_RoomItem>(GetOwningPlayer(), RoomItemWidgetClass);
			UUniformGridSlot* NewSlot = RoomListPanel->AddChildToUniformGrid(NewItem, 0, 1);
			NewItem->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}


void UUW_Lobby::OnCommitChatMessage(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		ANBPlayerController* NBPlayerController = GetOwningPlayer<ANBPlayerController>();
		if (IsValid(NBPlayerController))
		{
			FString NickName = NBPlayerController->GetNickName();
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

