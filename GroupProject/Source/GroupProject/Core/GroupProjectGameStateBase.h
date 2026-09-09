#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GroupProjectGameStateBase.generated.h"

UENUM(BlueprintType)
enum class EScenarioPhase : uint8
{
	Lobby,
	Starting,
	InProgress,
	Ended
};

UCLASS()
class GROUPPROJECT_API AGroupProjectGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AGroupProjectGameStateBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Group Project|Scenario")
	EScenarioPhase CurrentPhase;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Group Project|Scenario")
	FString ActiveScenarioName;
};
