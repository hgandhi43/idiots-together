#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "IdiotsTogetherObjectiveTrigger.generated.h"

class UBoxComponent;

UCLASS()
class IDIOTSTOGETHER_API AIdiotsTogetherObjectiveTrigger : public AActor
{
	GENERATED_BODY()

public:
	AIdiotsTogetherObjectiveTrigger();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* TriggerBox;

	/** Debug label for logs. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|Trigger")
	FName TriggerName;

	/** Objective (Objective.*) completed when a player enters the volume during Execution. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|Trigger", meta = (Categories = "Objective"))
	FGameplayTag ObjectiveTagToComplete;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|Trigger")
	bool bTriggerOnce;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Idiots Together|Trigger")
	bool bHasTriggered;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
