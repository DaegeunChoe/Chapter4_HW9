#include "UI/UW_Lobby.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"

UUW_Lobby::UUW_Lobby(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUW_Lobby::NativeConstruct()
{
	Super::NativeConstruct();
}

void UUW_Lobby::UpdatePlayerList(TArray<FString> NickNames)
{
	if (IsValid(UserListBox))
	{
		UserListBox->ClearChildren();
		for (FString& NickName : NickNames)
		{
			UTextBlock* TextBlockWidget = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
			TextBlockWidget->SetText(FText::FromString(NickName));
			UserListBox->AddChildToVerticalBox(TextBlockWidget);
		}
	}
}
