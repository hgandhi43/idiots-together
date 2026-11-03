#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IdiotsTogetherInteractableInterface.h"
#include "GameplayTagContainer.h"
#include "IdiotsTogetherTestBox.generated.h"

class UStaticMeshComponent;

UCLASS()
class IDIOTSTOGETHER_API AIdiotsTogetherTestBox : public AActor, public IIdiotsTogetherInteractableInterface
{
	GENERATED_BODY()

public:
	AIdiotsTogetherTestBox();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BoxMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|Interaction")
	FText InteractionPromptText;

	/** Objective (Objective.*) completed on interaction. Leave empty for a plain toggle box. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|Objective", meta = (Categories = "Objective"))
	FGameplayTag ObjectiveTagToComplete;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Idiots Together|State")
	bool bIsActivated;

	UPROPERTY(ReplicatedUsing = OnRep_TargetRotation)
	FRotator TargetRotation;

	FRotator InitialRotation;

	UFUNCTION()
	void OnRep_TargetRotation();

public:
	// IIdiotsTogetherInteractableInterface
	virtual FText GetInteractionPrompt_Implementation(APawn* InstigatorPawn) override;
	virtual bool CanInteract_Implementation(APawn* InstigatorPawn) override;
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	virtual void OnBeginFocus_Implementation(APawn* InstigatorPawn) override;
	virtual void OnEndFocus_Implementation(APawn* InstigatorPawn) override;
};
