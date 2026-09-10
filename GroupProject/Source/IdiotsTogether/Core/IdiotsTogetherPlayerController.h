#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IdiotsTogetherPlayerController.generated.h"

UCLASS()
class IDIOTSTOGETHER_API AIdiotsTogetherPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AIdiotsTogetherPlayerController();

	UFUNCTION(BlueprintCallable, Category = "Idiots Together|UI")
	void ShowInteractionPrompt(const FText& Text);

	UFUNCTION(BlueprintCallable, Category = "Idiots Together|UI")
	void HideInteractionPrompt();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|UI")
	TSubclassOf<class UIIdiotsTogetherInteractionWidget> InteractionWidgetClass;

	UPROPERTY()
	class UIIdiotsTogetherInteractionWidget* InteractionWidgetInstance;
};
