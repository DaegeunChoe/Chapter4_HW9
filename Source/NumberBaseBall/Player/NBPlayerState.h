#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NBPlayerState.generated.h"

UENUM(BlueprintType)
enum class EPlayerLocation : uint8
{
	None,
	Lobby,
	GameRoom,
	Max
};

USTRUCT()
struct FPlayerGameState
{
	GENERATED_BODY()

public:
	FPlayerGameState()
	{
		ReleaseTurn();
	}

	UPROPERTY()
	float RemainTime;

	UPROPERTY()
	bool HasTurn;

	void GetTurn(float Duration)
	{
		RemainTime = Duration;
		HasTurn = true;
	}
	void ReleaseTurn()
	{
		RemainTime = 0.0f;
		HasTurn = false;
	}
	bool ReduceTimeAndCheck(float DeltaTime)
	{
		RemainTime -= DeltaTime;
		if (RemainTime <= 0)
		{
			return true;
		}
		return false;
	}
};

UCLASS()
class NUMBERBASEBALL_API ANBPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ANBPlayerState();
	virtual void PostInitializeComponents() override;
	virtual void BeginDestroy() override;
	virtual void Destroyed() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void NotifyToLocalPlayerController();

	const FString& GetNickName() const { return NickName; }
	EPlayerLocation GetPlayerLocation() const { return PlayerLocation; }
	int32 GetRoomId() const { return RoomId; }

	void SetNickName(FString InNickName) { NickName = InNickName; }
	void SetPlayerLocationToLobby();
	void SetPlayerLocationToGameRoom(int32 InRoomId);

	FPlayerGameState* GetPlayerGameState() { return &PlayerGameState; }

protected:
	UPROPERTY(ReplicatedUsing = OnRep_NickName)
	FString NickName;

	UPROPERTY(Replicated)
	EPlayerLocation PlayerLocation;

	UPROPERTY(ReplicatedUsing = OnRep_RoomId)
	int32 RoomId;

	UPROPERTY(ReplicatedUsing = OnRep_PlayerGameState)
	FPlayerGameState PlayerGameState;

private:
	UFUNCTION()
	void OnRep_NickName();

	UFUNCTION()
	void OnRep_RoomId();

	UFUNCTION()
	void OnRep_PlayerGameState();

	virtual void OnRep_Owner() override;

	TArray<const FString> GetPlayerNickNames();

	void TryNotifyToLocalPlayerController();

	FTimerHandle LatePlayerStateUpdateHandle;
};
