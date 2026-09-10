#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IdiotsTogetherGameModeBase.generated.h"

UCLASS(minimalapi)
class AIdiotsTogetherGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AIdiotsTogetherGameModeBase();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Config")
	int32 MaxPlayers = 6;
};
