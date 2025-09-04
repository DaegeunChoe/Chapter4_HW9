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

FGameRoom* ANBGameStateBase::MakeGameRoom(ANBPlayerController* HostPlayer)
{
	FGameRoom NewRoom = FGameRoom::MakeGameRoom(HostPlayer);
	GameRooms.Add(MoveTemp(NewRoom));
	return &(GameRooms.Last());
}

void ANBGameStateBase::DestroyGameRoom(int32 TargetRoomId)
{
	for (int32 index = 0; index < GameRooms.Num(); index++)
	{
		FGameRoom& Room = GameRooms[index];
		if (Room.GetRoomId() == TargetRoomId)
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
		if (Room.GetRoomId() == TargetRoomId)
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

			for (FGameRoom& Room : GameRooms)
			{
				if (Room.IsHost(NBPlayerController) || Room.IsGuest(NBPlayerController))
				{
					NBPlayerController->UpdateGameRoomInfo(&Room);
				}
			}

			//if (ANBPlayerState* NBPlayerState = NBPlayerController->GetValidPlayerState<ANBPlayerState>())
			//{
			//	int32 RoomId = NBPlayerState->GetRoomId();
			//	GEngine->AddOnScreenDebugMessage(
			//		-1, 10, FColor::White, FString::Printf(TEXT("OnRep_GameRooms My RoomIs Id %d"), RoomId));
			//	FGameRoom* GameRoom = GetGameRoom(RoomId);
			//	if (GameRoom)
			//	{
			//		NBPlayerController->UpdateGameRoomInfo(GameRoom);
			//	}
			//}
		}
	}
}
