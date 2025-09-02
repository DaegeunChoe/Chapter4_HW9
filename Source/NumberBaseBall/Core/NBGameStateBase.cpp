#include "Core/NBGameStateBase.h"
#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ANBGameStateBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, GameRooms);
}

int32 ANBGameStateBase::MakeGameRoom(ANBPlayerController* HostPlayer)
{
	ANBPlayerState* NBPlayerState = HostPlayer->GetPlayerState<ANBPlayerState>();
	FString NickName = IsValid(NBPlayerState) ? NBPlayerState->GetNickName() : TEXT("NONE");
	
	FGameRoom& NewRoom = GameRooms.AddDefaulted_GetRef();
	NewRoom.RoomId = FGameRoom::NextRoomId++;
	NewRoom.Host = HostPlayer;

	FString FormatText = FString::Printf(TEXT("[%d] %s's Room"), NewRoom.RoomId, *NickName);
	NewRoom.RoomName = FText::FromString(FormatText);

	return NewRoom.RoomId;
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
			NBPlayerController->UpdateRoom(GameRooms);
		}
	}
}
