#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GroupProjectPlayerController.generated.h"

UCLASS()
class GROUPPROJECT_API AGroupProjectPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGroupProjectPlayerController();

	UFUNCTION(BlueprintCallable, Category = "Group Project|UI")
	void ShowInteractionPrompt(const FText& Text);

	UFUNCTION(BlueprintCallable, Category = "Group Project|UI")
	void HideInteractionPrompt();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Group Project|UI")
	TSubclassOf<class UGroupProjectInteractionWidget> InteractionWidgetClass;

	UPROPERTY()
	class UGroupProjectInteractionWidget* InteractionWidgetInstance;
};
