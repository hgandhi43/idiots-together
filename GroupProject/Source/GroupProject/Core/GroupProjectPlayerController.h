#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GroupProjectPlayerController.generated.h"

UCLASS()
class GROUPPROJECT_API AGroupProjectPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGroupProjectPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
};
