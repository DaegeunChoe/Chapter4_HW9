#include "Core/NBGameStateBase.h"
#include "Core/NBGameRoom.h"
#include "Player/Main/NBPlayerController.h"
#include "Player/Main/NBPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ANBGameStateBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, GameRooms);
}

int32 ANBGameStateBase::MakeGameRoom(ANBPlayerController* HostPlayer)
{
	FString NickName = HostPlayer->GetNickName();
	
	FGameRoom& NewRoom = GameRooms.AddDefaulted_GetRef();
	NewRoom.RoomId = FGameRoom::NextRoomId++;
	NewRoom.Host = HostPlayer;
	NewRoom.HostState = HostPlayer->GetValidPlayerState<ANBPlayerState>();

	FString FormatText = FString::Printf(TEXT("[%d] %s's Room"), NewRoom.RoomId, *NickName);
	NewRoom.RoomName = FText::FromString(FormatText);

	return NewRoom.RoomId;
}

bool ANBGameStateBase::JoinGameRoom(ANBPlayerController* GuestPlayer, int32 TargetRoomId)
{
	FGameRoom* GameRoom = GetGameRoom(TargetRoomId);
	if (GameRoom)
	{
		if (GameRoom->Guest)
		{
			return false;
		}
		else
		{
			GameRoom->Guest = GuestPlayer;
			GameRoom->GuestState = GuestPlayer->GetValidPlayerState<ANBPlayerState>();
			return true;
		}
	}
	return false;
}

void ANBGameStateBase::DestroyGameRoom(int32 TargetRoomId)
{
	for (int32 index = 0; index < GameRooms.Num(); index++)
	{
		FGameRoom& Room = GameRooms[index];
		if (Room.RoomId == TargetRoomId)
		{
			GameRooms.RemoveAt(index);
			break;
		}
	}
}

FGameRoom* ANBGameStateBase::GetGameRoom(int32 TargetRoomId)
{
	for (FGameRoom& Room : GameRooms)
	{
		if (Room.RoomId == TargetRoomId)
		{
			return &Room;
		}
	}
	return nullptr;
}

void ANBGameStateBase::OnRep_GameRooms()
{
	if (!HasAuthority())
	{
		APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(this, 0);
		ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(LocalPlayerController);
		if (IsValid(NBPlayerController))
		{
			NBPlayerController->UpdateRooms(GameRooms);

			if (ANBPlayerState* NBPlayerState = NBPlayerController->GetValidPlayerState<ANBPlayerState>())
			{
				int32 RoomId = NBPlayerState->GetRoomId();
				FGameRoom* GameRoom = GetGameRoom(RoomId);
				if (GameRoom)
				{
					NBPlayerController->UpdateGameRoomInfo(GameRoom);
				}
			}
		}
	}
}
