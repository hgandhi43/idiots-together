#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "IdiotsTogetherTypes.h"
#include "IdiotsTogetherGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRoundPhaseChanged, EIdiotsTogetherRoundPhase, NewPhase, EIdiotsTogetherRoundPhase, OldPhase);

UCLASS()
class IDIOTSTOGETHER_API AIdiotsTogetherGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AIdiotsTogetherGameStateBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentRoundPhase, BlueprintReadOnly, Category = "Idiots Together|Scenario")
	EIdiotsTogetherRoundPhase CurrentRoundPhase;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentRoundIndex, BlueprintReadOnly, Category = "Idiots Together|Scenario")
	int32 CurrentRoundIndex;

	UPROPERTY(ReplicatedUsing = OnRep_RemainingPhaseTime, BlueprintReadOnly, Category = "Idiots Together|Scenario")
	float RemainingPhaseTime;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveScenarioID, BlueprintReadOnly, Category = "Idiots Together|Scenario")
	FString ActiveScenarioID;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Idiots Together|Scenario")
	FText ScenarioDisplayName;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Idiots Together|Scenario")
	FText ScenarioDescription;

	UPROPERTY(BlueprintAssignable, Category = "Idiots Together|Scenario")
	FOnRoundPhaseChanged OnRoundPhaseChanged;

	UFUNCTION()
	void OnRep_CurrentRoundPhase(EIdiotsTogetherRoundPhase OldPhase);

	UFUNCTION()
	void OnRep_CurrentRoundIndex();

	UFUNCTION()
	void OnRep_RemainingPhaseTime();

	UFUNCTION()
	void OnRep_ActiveScenarioID();

	void SetCurrentRoundPhase_Server(EIdiotsTogetherRoundPhase NewPhase);
};
