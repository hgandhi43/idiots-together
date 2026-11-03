#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayTagContainer.h"
#include "IdiotsTogetherTypes.h"
#include "IdiotsTogetherGameModeBase.generated.h"

class UIdiotsTogetherScenarioDefinition;

UCLASS(minimalapi)
class AIdiotsTogetherGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AIdiotsTogetherGameModeBase();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	/** Server: gives the player the lowest free colour index from the project palette. */
	void AssignPlayerColor(APlayerController* NewPlayer);

	UFUNCTION(BlueprintCallable, Category = "Idiots Together|Scenario")
	void StartScenarioLifecycle();

	UFUNCTION(BlueprintCallable, Category = "Idiots Together|Scenario")
	void TransitionToPhase(EIdiotsTogetherRoundPhase NewPhase);

	UFUNCTION(BlueprintCallable, Category = "Idiots Together|Scenario")
	void AdvancePhase();

	UFUNCTION(BlueprintCallable, Category = "Idiots Together|Scenario")
	void ForcePhaseSuccess();

	UFUNCTION(BlueprintCallable, Category = "Idiots Together|Scenario")
	void ForcePhaseFailure();

	/** Server: marks an objective of the active scenario complete (Execution only). Ends the round early when all required objectives are done. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Idiots Together|Objective")
	bool CompleteObjective(FGameplayTag ObjectiveTag);

	bool CheckAllRequiredObjectivesCompleted() const;

	/** Dev builds: completes the first incomplete objective (required ones first). Returns false in Shipping. */
	bool DevCompleteNextObjective();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Config")
	int32 MaxPlayers = 6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Scenario")
	UIdiotsTogetherScenarioDefinition* ActiveScenarioDefinition;

	FTimerHandle PhaseTimerHandle;
	FTimerHandle CountdownTimerHandle;
	float CurrentPhaseDuration = 0.0f;
	float PhaseTimeRemaining = 0.0f;

	void OnPhaseTimerExpired();
	void UpdatePhaseCountdown();
};
