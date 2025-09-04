#include "Player/Main/NBPlayerState.h"
#include "Player/Main/NBPlayerController.h"
#include "Core/NBGameStateBase.h"
#include "Core/NBGameRoom.h"
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

	if (LatePlayerStateUpdateHandle.IsValid())
	{
		LatePlayerStateUpdateHandle.Invalidate();
	}

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
	DOREPLIFETIME(ThisClass, RoomId);
	DOREPLIFETIME(ThisClass, PlayerGameState);
}


void ANBPlayerState::OnRep_NickName()
{
	NotifyToLocalPlayerController();
}

void ANBPlayerState::OnRep_RoomId()
{
	NotifyToLocalPlayerController();
}

void ANBPlayerState::OnRep_PlayerGameState()
{
	TryNotifyToLocalPlayerController();
}

void ANBPlayerState::OnRep_Owner()
{
	NotifyToLocalPlayerController();
}

void ANBPlayerState::NotifyToLocalPlayerController()
{
	if (!HasAuthority())
	{
		if (GetWorld())
		{
			if (LatePlayerStateUpdateHandle.IsValid())
			{
				GetWorldTimerManager().ClearTimer(LatePlayerStateUpdateHandle);
			}

			GetWorldTimerManager().SetTimer(LatePlayerStateUpdateHandle, this, &ThisClass::TryNotifyToLocalPlayerController, 0.5f, true);
		}
	}
}

void ANBPlayerState::TryNotifyToLocalPlayerController()
{
	if (!HasAuthority())
	{
		bool HasUpdate = true;
		APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(this, 0);
		ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(LocalPlayerController);
		if (IsValid(NBPlayerController))
		{
			NBPlayerController->UpdatePlayerList(GetPlayerNickNames());

			UWorld* World = GetWorld();
			if (RoomId != -1 && IsValid(World))
			{
				ANBGameStateBase* NBGameStateBase = World->GetGameState<ANBGameStateBase>();
				if (IsValid(NBGameStateBase))
				{
					FGameRoom* GameRoom = NBGameStateBase->GetGameRoom(RoomId);
					if (GameRoom)
					{
						if (GameRoom->IsHost(NBPlayerController) || GameRoom->IsGuest(NBPlayerController))
						{
							NBPlayerController->UpdateGameRoomInfo(GameRoom);
						}
					}
					else
					{
						HasUpdate = false;
					}
				}
				else
				{
					HasUpdate = false;
				}
			}
		}
		else
		{
			HasUpdate = false;
		}

		ANBPlayerController* OwnerPlayerController = GetOwner<ANBPlayerController>();
		if (IsValid(OwnerPlayerController))
		{
			OwnerPlayerController->UpdateMyNickName(GetNickName());
		}
		
		if (HasUpdate)
		{
			GetWorldTimerManager().ClearTimer(LatePlayerStateUpdateHandle);
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
