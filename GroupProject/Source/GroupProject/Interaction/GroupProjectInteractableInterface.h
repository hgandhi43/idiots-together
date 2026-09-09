#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GroupProjectInteractableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UGroupProjectInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class GROUPPROJECT_API IGroupProjectInteractableInterface
{
	GENERATED_BODY()

public:
	/** Returns the prompt text to display on the UI when focusing this object (e.g. "Press E to Interact") */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Group Project|Interaction")
	FText GetInteractionPrompt(APawn* InstigatorPawn);

	/** Returns true if the object can currently be interacted with */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Group Project|Interaction")
	bool CanInteract(APawn* InstigatorPawn);

	/** Executes the interaction action */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Group Project|Interaction")
	void Interact(APawn* InstigatorPawn);

	/** Called when the player starts looking at this object */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Group Project|Interaction")
	void OnBeginFocus(APawn* InstigatorPawn);

	/** Called when the player stops looking at this object */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Group Project|Interaction")
	void OnEndFocus(APawn* InstigatorPawn);
};
