#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|Trigger")
	FString TriggerID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|Trigger")
	FString ObjectiveIDToComplete;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|Trigger")
	bool bTriggerOnce;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Idiots Together|Trigger")
	bool bHasTriggered;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
