#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "IdiotsTogetherTypes.h"
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

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Idiots Together|Character")
	void ServerRequestCharacterSelection(EIdiotsTogetherCharacterType Selection);

	void DevSelectBoss();
	void DevSelectNerd();
	void DevSelectMuscle();
	void DevSelectIntern();
	void DevSelectRandom();
	void ToggleDevCursor();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|UI")
	TSubclassOf<class UIIdiotsTogetherInteractionWidget> InteractionWidgetClass;

	UPROPERTY()
	class UIIdiotsTogetherInteractionWidget* InteractionWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|UI")
	TSubclassOf<class UIdiotsTogetherDevCharSelectWidget> DevCharSelectWidgetClass;

	UPROPERTY()
	class UIdiotsTogetherDevCharSelectWidget* DevCharSelectWidgetInstance;
};
