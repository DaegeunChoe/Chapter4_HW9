#include "UI/UW_Login.h"
#include "Player/NBLoginPlayerController.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/KismetSystemLibrary.h"

UUW_Login::UUW_Login(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UUW_Login::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(LoginButton))
	{
		LoginButton->OnClicked.AddDynamic(this, &ThisClass::OnLoginButtonClicked);
	}
	if (IsValid(ExitButton))
	{
		ExitButton->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
	}
}

void UUW_Login::OnLoginButtonClicked()
{
	ANBLoginPlayerController* LoginPlayerController = GetOwningPlayer<ANBLoginPlayerController>();
	if (IsValid(LoginPlayerController))
	{
		if (IsValid(ServerIPEditableTextBox) && IsValid(NickNameTextBox))
		{
			FText ServerIP = ServerIPEditableTextBox->GetText();
			FText NickName = NickNameTextBox->GetText();
			LoginPlayerController->JoinServer(ServerIP, NickName);
		}
	}
}

void UUW_Login::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
