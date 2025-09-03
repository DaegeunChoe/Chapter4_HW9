#include "Player/Login/NBLoginPlayerController.h"
#include "UI/UW_Login.h"
#include "Kismet/GameplayStatics.h"


void ANBLoginPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		LoginWidgetInstance = CheckAndCreateWidget<UUW_Login>(LoginWidgetClass);
		AddToViewportAndSetInputMode(LoginWidgetInstance);
	}
}

void ANBLoginPlayerController::JoinServer(const FText& ServerIP, const FText& NickName)
{
	FName ServerURL = FName(ServerIP.ToString());
	FString Option = FString::Printf(TEXT("nickname=%s"), *NickName.ToString());
	UGameplayStatics::OpenLevel(GetWorld(), ServerURL, true, Option);
}

void ANBLoginPlayerController::AddToViewportAndSetInputMode(UUserWidget* TargetWidget)
{
	if (IsValid(TargetWidget))
	{
		TargetWidget->AddToViewport(0);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TargetWidget->GetCachedWidget());
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
}
