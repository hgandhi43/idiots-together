#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "IdiotsTogetherObjectiveComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class IDIOTSTOGETHER_API UIdiotsTogetherObjectiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UIdiotsTogetherObjectiveComponent();

	/** Objective (Objective.*) completed when TriggerObjectiveCompletion is called on the server. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|Objective", meta = (Categories = "Objective"))
	FGameplayTag ObjectiveTagToComplete;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|Objective")
	bool bTriggerOnce = true;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Idiots Together|Objective")
	bool bHasTriggered = false;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Idiots Together|Objective")
	bool TriggerObjectiveCompletion(APawn* InstigatorPawn);
};
