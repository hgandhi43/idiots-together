#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "IdiotsTogetherPlayerState.generated.h"

UCLASS()
class IDIOTSTOGETHER_API AIdiotsTogetherPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AIdiotsTogetherPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Idiots Together|Player")
	bool bIsReady;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Idiots Together|Player")
	int32 PlayerColorIndex;
};
