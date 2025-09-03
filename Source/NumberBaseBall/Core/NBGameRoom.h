#pragma once

#include "NBGameRoom.generated.h"

class ANBPlayerController;
class ANBPlayerState;

USTRUCT()
struct FGameRoom
{
	GENERATED_BODY()

public:
	FGameRoom()
	{
		RoomId = -1;
		IsPlaying = false;
		Host = nullptr;
		HostState = nullptr;
		Guest = nullptr;
		GuestState = nullptr;
	}

	static FGameRoom MakeGameRoom(ANBPlayerController* HostPlayer);

	void JoinGuestInRoom(ANBPlayerController* GuestPlayer);
	void LeaveGuest();
	void StartGame();
	void EndGame();
	TArray<ANBPlayerController*> GetPlayersInRoom() const;

	int32 GetRoomId() const { return RoomId; }
	const FText& GetRoomName() const { return RoomName; }
	
	ANBPlayerController* GetHost() const { return Host; }
	ANBPlayerController* GetGuest() const { return Guest; }
	ANBPlayerState* GetHostState() const { return HostState; }
	ANBPlayerState* GetGuestState() const { return GuestState; }

	bool IsHost(ANBPlayerController* Player) const { return Player == Host; }
	bool IsGuest(ANBPlayerController* Player) const { return Player == Guest; }
	bool IsPlayingRoom() const { return IsPlaying; }
	bool CanJoin() const
	{
		return (Guest == nullptr) && (!IsPlaying);
	}

	bool HostHasTurn();
	bool GuestHasTurn();

private:
	static int32 NextRoomId;

	UPROPERTY()
	int32 RoomId;

	UPROPERTY()
	FText RoomName;

	UPROPERTY()
	bool IsPlaying;

	UPROPERTY()
	TObjectPtr<ANBPlayerController> Host;

	UPROPERTY()
	TObjectPtr<ANBPlayerController> Guest;

	UPROPERTY()
	TObjectPtr<ANBPlayerState> HostState;

	UPROPERTY()
	TObjectPtr<ANBPlayerState> GuestState;
};
