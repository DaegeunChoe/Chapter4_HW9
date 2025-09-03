#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NBGameStateBase.generated.h"

class ANBPlayerController;
struct FGameRoom;

UCLASS()
class NUMBERBASEBALL_API ANBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	int32 MakeGameRoom(ANBPlayerController* HostPlayer);
	bool JoinGameRoom(ANBPlayerController* GuestPlayer, int32 TargetRoomId);
	void DestroyGameRoom(int32 TargetRoomId);
	TArray<FGameRoom>& GetGameRoomsRef() { return GameRooms; }
	FGameRoom* GetGameRoom(int32 TargetRoomId);

private:
	UPROPERTY(ReplicatedUsing=OnRep_GameRooms)
	TArray<FGameRoom> GameRooms;

	UFUNCTION()
	void OnRep_GameRooms();
};
