#include "Player/Main/NBPlayerController.h"
#include "Player/Main/NBPlayerState.h"
#include "Core/NBGameModeBase.h"
#include "Core/NBGameStateBase.h"
#include "Core/NBGameRoom.h"

void ANBPlayerController::ServerRPCSendChatMessage_Implementation(const FText& ChatMessage)
{
	if (HasAuthority())
	{
		if (ANBPlayerState* NBPlayerState = GetValidPlayerState<ANBPlayerState>())
		{
			if (NBPlayerState->GetPlayerLocation() == EPlayerLocation::Lobby)
			{
				BroadcastToLobby(ChatMessage);
			}
			else if (NBPlayerState->GetPlayerLocation() == EPlayerLocation::GameRoom)
			{
				BroadcstToGameRoom(ChatMessage);
			}
		}
	}
}

void ANBPlayerController::ServerRPCMakeRoom_Implementation()
{
	if (HasAuthority())
	{
		if (ANBGameModeBase* NBGameModeBase = GetValidGameMode<ANBGameModeBase>())
		{
			NBGameModeBase->MakeRoom(this);
		}
	}
}

void ANBPlayerController::ServerRPCJoinRoom_Implementation(int32 RoomId)
{
	if (HasAuthority())
	{
		if (ANBGameModeBase* NBGameModeBase = GetValidGameMode<ANBGameModeBase>())
		{
			NBGameModeBase->JoinRoom(this, RoomId);
		}
	}
}

void ANBPlayerController::ServerRPCLeaveRoom_Implementation()
{
	if (HasAuthority())
	{
		if (ANBGameModeBase* NBGameModeBase = GetValidGameMode<ANBGameModeBase>())
		{
			NBGameModeBase->LeaveRoom(this);
		}
	}
}

void ANBPlayerController::ServerRPCStartGame_Implementation()
{
	if (HasAuthority())
	{
		if (ANBGameModeBase* NBGameModeBase = GetValidGameMode<ANBGameModeBase>())
		{
			NBGameModeBase->StartGame(this);
		}
	}
}

void ANBPlayerController::ServerRPCSendGuessNumber_Implementation(const FText& GuessNumberText)
{
	if (HasAuthority())
	{
		if (ANBGameModeBase* NBGameModeBase = GetValidGameMode<ANBGameModeBase>())
		{
			NBGameModeBase->GuessNumber(this, GuessNumberText);
		}
	}
}

void ANBPlayerController::BroadcastToLobby(const FText& ChatMessage)
{
	if (HasAuthority())
	{
		if (ANBGameModeBase* NBGameModeBase = GetValidGameMode<ANBGameModeBase>())
		{
			TArray<TObjectPtr<ANBPlayerController>> PlayerList = NBGameModeBase->GetPlayersInLobby();
			for (ANBPlayerController* PlayerController : PlayerList)
			{
				PlayerController->ClientRPCReceiveChatMessage(ChatMessage);
			}
		}
	}
}

void ANBPlayerController::BroadcstToGameRoom(const FText& ChatMessage)
{
	if (HasAuthority())
	{
		if (ANBGameStateBase* NBGameStateBase = GetValidGameState<ANBGameStateBase>())
		{
			if (ANBPlayerState* NBPlayerState = GetValidPlayerState<ANBPlayerState>())
			{
				int32 RoomId = NBPlayerState->GetRoomId();
				FGameRoom* GameRoom = NBGameStateBase->GetGameRoom(RoomId);

				TArray<ANBPlayerController*> PlayersInRoom = GameRoom->GetPlayersInRoom();
				for (ANBPlayerController* PlayerInRoom : PlayersInRoom)
				{
					if (IsValid(PlayerInRoom))
					{
						PlayerInRoom->ClientRPCReceiveChatMessage(ChatMessage);
					}
				}
			}
		}
	}
}

