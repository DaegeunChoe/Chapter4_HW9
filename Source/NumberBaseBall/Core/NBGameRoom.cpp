#include "Core/NBGameRoom.h"
#include "Player/Main/NBPlayerController.h"

int32 FGameRoom::NextRoomId = 0;

FGameRoom FGameRoom::MakeGameRoom(ANBPlayerController* HostPlayer)
{
	int32 NewRoomId = FGameRoom::NextRoomId++;
	FString NickName = HostPlayer->GetNickName();
	FString FormatText = FString::Printf(TEXT("[%d] %s's Room"), NewRoomId, *NickName);

	FGameRoom NewRoom;
	NewRoom.RoomId = NewRoomId;
	NewRoom.Host = HostPlayer;
	NewRoom.HostState = HostPlayer->GetValidPlayerState<ANBPlayerState>();
	NewRoom.RoomName = FText::FromString(FormatText);

	return NewRoom;
}

void FGameRoom::JoinGuestInRoom(ANBPlayerController* GuestPlayer)
{
	Guest = GuestPlayer;
	GuestState = GuestPlayer->GetValidPlayerState<ANBPlayerState>();
}

void FGameRoom::LeaveGuest()
{
	Guest = nullptr;
	GuestState = nullptr;
}

void FGameRoom::StartGame()
{
	IsPlaying = true;
}

void FGameRoom::EndGame()
{
	IsPlaying = false;
}

TArray<ANBPlayerController*> FGameRoom::GetPlayersInRoom() const
{
	TArray<ANBPlayerController*> PlayersInRoom;
	if (Host)
	{
		PlayersInRoom.Add(Host);
	}
	if (Guest)
	{
		PlayersInRoom.Add(Guest);
	}
	return PlayersInRoom;
}

bool FGameRoom::HostHasTurn()
{
	if (IsValid(HostState))
	{
		if (HostState->GetPlayerGameState())
		{
			return HostState->GetPlayerGameState()->HasTurn;
		}
	}
	return false;
}

bool FGameRoom::GuestHasTurn()
{
	if (IsValid(GuestState))
	{
		if (GuestState->GetPlayerGameState())
		{
			return GuestState->GetPlayerGameState()->HasTurn;
		}
	}
	return false;
}
