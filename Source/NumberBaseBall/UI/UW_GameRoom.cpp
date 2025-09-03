#include "UI/UW_GameRoom.h"
#include "Core/NBGameStateBase.h"
#include "Player/Main/NBPlayerController.h"
#include "Player/Main/NBPlayerState.h"
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

	if (IsValid(GameStartButton))
	{
		bool IsAlreadyBounded = GameStartButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnGameStartButtonClicked);;
		if (!IsAlreadyBounded)
		{
			GameStartButton->OnClicked.AddDynamic(this, &ThisClass::OnGameStartButtonClicked);
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

	if (IsValid(GameStartButton))
	{
		bool IsAlreadyBounded = GameStartButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnGameStartButtonClicked);;
		if (IsAlreadyBounded)
		{
			GameStartButton->OnClicked.RemoveDynamic(this, &ThisClass::OnGameStartButtonClicked);
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
	int32 PlayerCountInRoom = 0;
	if (IsValid(HostTextBlock))
	{
		FString HostNickName;
		if (GameRoomInfo)
		{
			if (IsValid(GameRoomInfo->HostState))
			{
				HostNickName = GameRoomInfo->HostState->GetNickName();
				PlayerCountInRoom++;
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
				PlayerCountInRoom++;
			}
		}
		FString FormatText = FString::Printf(TEXT("Guest: %s"), *GuestNickName);

		FText GuestText = FText::FromString(FormatText);
		GuestTextBlock->SetText(GuestText);
	}

	if (IsValid(GameStartButton))
	{
		if (PlayerCountInRoom >= 2 && !GameRoomInfo->IsPlaying)
		{
			if (GameRoomInfo->Host == GetOwningPlayer())
			{
				GameStartButton->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			GameStartButton->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (IsValid(ExitRoomButton))
	{
		if (GameRoomInfo->IsPlaying)
		{
			ExitRoomButton->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			ExitRoomButton->SetVisibility(ESlateVisibility::Visible);
		}
	}

	ANBPlayerController* NBPlayerController = GetOwningPlayer<ANBPlayerController>();
	if (IsValid(NBPlayerController))
	{
		if (ANBPlayerState* NBPlayerState = NBPlayerController->GetValidPlayerState<ANBPlayerState>())
		{
			if (GameRoomInfo)
			{
				if (GameRoomInfo->IsPlaying)
				{
					CachedRemainTime = NBPlayerState->GetPlayerGameState()->RemainTime;
					CachedRemainTime = FMath::Clamp(CachedRemainTime, 0.0f, CachedRemainTime);
					SetTimerText();
				}
				else
				{
					if (IsValid(TimerTextBlock))
					{
						TimerTextBlock->SetVisibility(ESlateVisibility::Hidden);
					}
				}
			}
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
			bool IsGuessNumber = IsGuessNumberString(Text.ToString());
			if (IsGuessNumber)
			{
				NBPlayerController->ServerRPCSendGuessNumber(Text);
			}
			else
			{
				ANBPlayerState* NBPlayerState = NBPlayerController->GetValidPlayerState<ANBPlayerState>();
				FString NickName = IsValid(NBPlayerState) ? NBPlayerState->GetNickName() : "NONE";
				FString FormatString = FString::Printf(TEXT("%s: %s"), *NickName, *Text.ToString());
				FText ChatMessage = FText::FromString(FormatString);
				NBPlayerController->ServerRPCSendChatMessage(ChatMessage);
			}
		}
		if (IsValid(InputEditableTextBox))
		{
			InputEditableTextBox->SetText(FText());
		}
	}
}

void UUW_GameRoom::OnGameStartButtonClicked()
{
	ANBPlayerController* NBPlayerController = GetOwningPlayer<ANBPlayerController>();
	if (IsValid(NBPlayerController))
	{
		NBPlayerController->ServerRPCStartGame();
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

void UUW_GameRoom::SetTimerText()
{
	if (IsValid(TimerTextBlock))
	{
		float RemainTime = CachedRemainTime;
		ANBPlayerController* NBPlayerController = GetOwningPlayer<ANBPlayerController>();
		if (IsValid(NBPlayerController))
		{
			if (ANBPlayerState* NBPlayerState = NBPlayerController->GetValidPlayerState<ANBPlayerState>())
			{
				RemainTime = NBPlayerState->GetPlayerGameState()->RemainTime;
				RemainTime = FMath::Clamp(RemainTime, 0.0f, RemainTime);
			}
		}
		FString TimeString = FString::Printf(TEXT("Time: %.2f"), RemainTime);
		FText TimeText = FText::FromString(TimeString);

		TimerTextBlock->SetVisibility(ESlateVisibility::Visible);
		TimerTextBlock->SetText(TimeText);
	}
}

bool UUW_GameRoom::IsGuessNumberString(const FString& InNumberString)
{
	if (InNumberString.Len() != 3)
	{
		return false;
	}
	else
	{
		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				return false;
			}
			else
			{
				UniqueDigits.Add(C);
			}
		}
		if (UniqueDigits.Num() == 3)
		{
			return true;
		}
	}
	return false;
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
