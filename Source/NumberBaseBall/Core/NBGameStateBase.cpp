#include "Core/NBGameStateBase.h"
#include "Player/NBPlayerController.h"
#include "Player/NBPlayerState.h"

void ANBGameStateBase::MakeGameRoom(ANBPlayerController* HostPlayer)
{
	ANBPlayerState* NBPlayerState = HostPlayer->GetPlayerState<ANBPlayerState>();
	FString NickName = IsValid(NBPlayerState) ? NBPlayerState->GetNickName() : TEXT("NONE");
	FString FormatText = FString::Printf(TEXT("%s's Room"), *NickName);

	FGameRoom& NewRoom = GameRooms.AddDefaulted_GetRef();
	NewRoom.RoomId = FGameRoom::NextRoomId++;
	NewRoom.Host = HostPlayer;
	NewRoom.RoomName = FText::FromString(FormatText);
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
