#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "IdiotsTogetherTypes.h"
#include "IdiotsTogetherScenarioDefinition.generated.h"

UCLASS(BlueprintType, Blueprintable)
class IDIOTSTOGETHER_API UIdiotsTogetherScenarioDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UIdiotsTogetherScenarioDefinition();

	/** Unique scenario ID (Scenario.*). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario", meta = (Categories = "Scenario"))
	FGameplayTag ScenarioTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario|Timing", meta = (ClampMin = "0", Units = "s"))
	float BriefingDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario|Timing", meta = (ClampMin = "0", Units = "s"))
	float PlanningDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario|Timing", meta = (ClampMin = "0", Units = "s"))
	float ExecutionDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario|Timing", meta = (ClampMin = "0", Units = "s"))
	float ResultsDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario|Rounds", meta = (ClampMin = "1"))
	int32 TargetRoundCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario|Objectives")
	TArray<FIdiotsTogetherObjectiveDefinition> Objectives;
};
