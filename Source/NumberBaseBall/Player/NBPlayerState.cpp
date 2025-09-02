#include "Player/NBPlayerState.h"
#include "Player/NBPlayerController.h"
#include "Core/NBGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ANBPlayerState::ANBPlayerState()
{
	PlayerLocation = EPlayerLocation::None;
	RoomId = -1;
}

void ANBPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ANBPlayerState::BeginDestroy()
{
	Super::BeginDestroy();

	NotifyToLocalPlayerController();
}

void ANBPlayerState::Destroyed()
{
	Super::Destroyed();

	NotifyToLocalPlayerController();
}

void ANBPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NickName);
}


void ANBPlayerState::OnRep_NickName()
{
	NotifyToLocalPlayerController();
}

void ANBPlayerState::OnRep_Owner()
{
	NotifyToLocalPlayerController();
}

void ANBPlayerState::NotifyToLocalPlayerController()
{
	if (!HasAuthority())
	{
		{
			APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(this, 0);
			ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(LocalPlayerController);
			if (IsValid(NBPlayerController))
			{
				NBPlayerController->UpdatePlayerList(GetPlayerNickNames());
			}
		}
		{
			ANBPlayerController* NBPlayerController = GetOwner<ANBPlayerController>();
			if (IsValid(NBPlayerController))
			{
				NBPlayerController->UpdateMyNickName(GetNickName());
			}
		}
	}
}

void ANBPlayerState::SetPlayerLocationToLobby()
{
	PlayerLocation = EPlayerLocation::Lobby;
	RoomId = -1;
}

void ANBPlayerState::SetPlayerLocationToGameRoom(int32 InRoomId)
{
	PlayerLocation = EPlayerLocation::GameRoom;
	RoomId = InRoomId;
}

TArray<const FString> ANBPlayerState::GetPlayerNickNames()
{
	TArray<const FString> NickNameArray;

	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		ANBGameStateBase* NBGameStateBase = World->GetGameState<ANBGameStateBase>();
		if (IsValid(NBGameStateBase))
		{
			for (auto& OtherPlayerState : NBGameStateBase->PlayerArray)
			{
				ANBPlayerState* NBPlayerState = Cast<ANBPlayerState>(OtherPlayerState);
				if (IsValid(NBPlayerState))
				{
					NickNameArray.Emplace(NBPlayerState->GetNickName());
				}
			}
		}
	}
	NickNameArray.Sort();
	return NickNameArray;
}
