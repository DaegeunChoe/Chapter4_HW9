#include "Player/NBPlayerState.h"
#include "Player/NBPlayerController.h"


void ANBPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!HasAuthority())
	{
		ANBPlayerController* PlayerController = GetOwner<ANBPlayerController>();
		GEngine->AddOnScreenDebugMessage(-1, 30, FColor::Blue, FString::Printf(TEXT("ANBPlayerState::PostInitializeComponents()")));
		if (IsValid(PlayerController))
		{
			// TODO: 이게 호출이 안됨
			// Owner Replication을 기다려야 할 듯
			PlayerController->UpdatePlayerList();
			GEngine->AddOnScreenDebugMessage(-1, 30, FColor::Blue, FString::Printf(TEXT("PlayerController->UpdatePlayerList()")));
		}
	}
}

void ANBPlayerState::BeginDestroy()
{
	Super::BeginDestroy();

	if (!HasAuthority())
	{
		ANBPlayerController* PlayerController = GetOwner<ANBPlayerController>();
		GEngine->AddOnScreenDebugMessage(-1, 30, FColor::Blue, FString::Printf(TEXT(" ANBPlayerState::BeginDestroy()")));
		if (IsValid(PlayerController))
		{
			PlayerController->UpdatePlayerList();
			GEngine->AddOnScreenDebugMessage(-1, 30, FColor::Blue, FString::Printf(TEXT("PlayerController->UpdatePlayerList()")));
		}
	}
}
