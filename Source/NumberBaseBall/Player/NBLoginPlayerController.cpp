#include "Player/NBLoginPlayerController.h"
#include "UI/UW_Login.h"
#include "Kismet/GameplayStatics.h"


void ANBLoginPlayerController::BeginPlay()
{
	Super::BeginPlay();

	LoginWidgetInstance = CreateLoginWidget();
	AddToViewportAndSetInputMode(LoginWidgetInstance);
}

void ANBLoginPlayerController::JoinServer(const FText& ServerIP, const FText& NickName)
{
	FName ServerURL = FName(ServerIP.ToString());
	FString Option = FString::Printf(TEXT("nickname=%s"), *NickName.ToString());
	UGameplayStatics::OpenLevel(GetWorld(), ServerURL, true, Option);
}

UUW_Login* ANBLoginPlayerController::CreateLoginWidget()
{
	if (IsValid(LoginWidgetClass))
	{
		UUW_Login* WidgetInstance = CreateWidget<UUW_Login>(this, LoginWidgetClass);
		if (IsValid(WidgetInstance))
		{
			WidgetInstance->AddToViewport(0);
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(WidgetInstance->GetCachedWidget());
			SetInputMode(InputMode);
		}
		return WidgetInstance;
	}
	return nullptr;
}

void ANBLoginPlayerController::AddToViewportAndSetInputMode(UUserWidget* TargetWidget)
{
	if (IsValid(TargetWidget))
	{
		TargetWidget->AddToViewport(0);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TargetWidget->GetCachedWidget());
		SetInputMode(InputMode);
	}
}
