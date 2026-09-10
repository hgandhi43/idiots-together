#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GroupProjectInteractableInterface.h"
#include "GroupProjectTestBox.generated.h"

class UStaticMeshComponent;

UCLASS()
class GROUPPROJECT_API AGroupProjectTestBox : public AActor, public IGroupProjectInteractableInterface
{
	GENERATED_BODY()

public:
	AGroupProjectTestBox();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BoxMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Group Project|Interaction")
	FText InteractionPromptText;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Group Project|State")
	bool bIsActivated;

	UPROPERTY(ReplicatedUsing = OnRep_TargetRotation)
	FRotator TargetRotation;

	FRotator InitialRotation;

	UFUNCTION()
	void OnRep_TargetRotation();

public:
	// IGroupProjectInteractableInterface
	virtual FText GetInteractionPrompt_Implementation(APawn* InstigatorPawn) override;
	virtual bool CanInteract_Implementation(APawn* InstigatorPawn) override;
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	virtual void OnBeginFocus_Implementation(APawn* InstigatorPawn) override;
	virtual void OnEndFocus_Implementation(APawn* InstigatorPawn) override;
};
