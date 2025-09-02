#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_Lobby.generated.h"

class UVerticalBox;

UCLASS()
class NUMBERBASEBALL_API UUW_Lobby : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UUW_Lobby(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void NativeConstruct() override;

	void UpdatePlayerList(TArray<FString> NickNames);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> UserListBox;

	
};
