#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "IdiotsTogetherTypes.h"
#include "IdiotsTogetherScenarioDefinition.generated.h"

UCLASS(BlueprintType, Blueprintable)
class IDIOTSTOGETHER_API UIdiotsTogetherScenarioDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UIdiotsTogetherScenarioDefinition();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario")
	FString ScenarioID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario|Timing")
	float BriefingDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario|Timing")
	float PlanningDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario|Timing")
	float ExecutionDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario|Timing")
	float ResultsDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario|Rounds")
	int32 TargetRoundCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario|Objectives")
	TArray<FIdiotsTogetherObjectiveDefinition> Objectives;
};
