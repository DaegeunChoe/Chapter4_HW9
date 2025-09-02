#include "Player/NBPlayerState.h"
#include "Player/NBPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ANBPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ANBPlayerState::BeginDestroy()
{
	Super::BeginDestroy();

	NotifyToLocalPlayerController();
}

void ANBPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NickName);
}

void ANBPlayerState::NotifyToLocalPlayerController()
{
	if (!HasAuthority())
	{
		APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(this, 0);
		ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(LocalPlayerController);
		if (IsValid(NBPlayerController))
		{
			NBPlayerController->UpdatePlayerList();
		}
	}
}

void ANBPlayerState::OnRep_NickName()
{
	NotifyToLocalPlayerController();
}
