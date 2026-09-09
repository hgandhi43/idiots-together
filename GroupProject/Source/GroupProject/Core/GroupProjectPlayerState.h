#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GroupProjectPlayerState.generated.h"

UCLASS()
class GROUPPROJECT_API AGroupProjectPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AGroupProjectPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Group Project|Player")
	bool bIsReady;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Group Project|Player")
	int32 PlayerColorIndex;
};
