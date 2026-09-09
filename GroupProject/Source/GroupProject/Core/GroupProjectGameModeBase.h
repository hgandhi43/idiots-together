#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GroupProjectGameModeBase.generated.h"

UCLASS(minimalapi)
class AGroupProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGroupProjectGameModeBase();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Group Project|Config")
	int32 MaxPlayers = 6;
};
