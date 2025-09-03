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
