#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IdiotsTogetherInteractableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UIIdiotsTogetherInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class IDIOTSTOGETHER_API IIIdiotsTogetherInteractableInterface
{
	GENERATED_BODY()

public:
	/** Returns the prompt text to display on the UI when focusing this object (e.g. "Press E to Interact") */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Idiots Together|Interaction")
	FText GetInteractionPrompt(APawn* InstigatorPawn);

	/** Returns true if the object can currently be interacted with */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Idiots Together|Interaction")
	bool CanInteract(APawn* InstigatorPawn);

	/** Executes the interaction action */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Idiots Together|Interaction")
	void Interact(APawn* InstigatorPawn);

	/** Called when the player starts looking at this object */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Idiots Together|Interaction")
	void OnBeginFocus(APawn* InstigatorPawn);

	/** Called when the player stops looking at this object */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Idiots Together|Interaction")
	void OnEndFocus(APawn* InstigatorPawn);
};
