#include "Core/NBGameModeBase.h"
#include "Core/NBGameStateBase.h"
#include "Player/NBPlayerController.h"

APlayerController* ANBGameModeBase::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	SetNickNameFromOptions(NewPlayerController, Options);
	return NewPlayerController;
}

void ANBGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (!PlayerList.Contains(NewPlayer))
	{
		PlayerList.Add(NewPlayer);
	}
}

void ANBGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (APlayerController* PlayerController = Cast<APlayerController>(Exiting))
	{
		if (PlayerList.Contains(PlayerController))
		{
			PlayerList.Remove(PlayerController);
		}
	}
}

void ANBGameModeBase::SetNickNameFromOptions(APlayerController* PlayerController, const FString& Options)
{
	ANBPlayerController* NBPlayerController = Cast<ANBPlayerController>(PlayerController);
	if (IsValid(NBPlayerController))
	{
		UE_LOG(LogTemp, Log, TEXT("ANBGameModeBase::Login, Options: %s"), *Options);

		FString NickName;
		FParse::Value(*Options, TEXT("nickname="), NickName);
		NBPlayerController->SetNickName(NickName); // TODO: 서버에서 지정한 NickName은 클라이언트에 아직 전달되지 않는다.
		// TODO: 애초에 지금 컨트롤러랑 스테이트 양 쪽에 닉네임이 있는 것도 문제다.
	}
}