#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NBGameStateBase.generated.h"

class ANBPlayerController;

USTRUCT()
struct FGameRoom
{
	GENERATED_BODY()

public:
	FGameRoom()
	{
		RoomId = -1;
		Host = nullptr;
		Guest = nullptr;
	}

	static int32 NextRoomId;

	UPROPERTY()
	int32 RoomId;

	UPROPERTY()
	FText RoomName;

	UPROPERTY()
	TObjectPtr<ANBPlayerController> Host;

	UPROPERTY()
	TObjectPtr<ANBPlayerController> Guest;
};

UCLASS()
class NUMBERBASEBALL_API ANBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	int32 MakeGameRoom(ANBPlayerController* HostPlayer);
	void DestroyGameRoom(int32 TargetRoomId);
	TArray<FGameRoom>& GetGameRoomsRef() { return GameRooms; }

private:
	UPROPERTY()
	TArray<FGameRoom> GameRooms;
};
