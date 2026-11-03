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

	// M3B Debug Phase Controls (Keys 6, 7, 8)
	void DevAdvancePhase();
	void DevForceSuccess();
	void DevForceFailure();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDevAdvancePhase();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDevForceSuccess();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDevForceFailure();

	// M3D Debug Objective Controls (Key 9)
	void DevCompleteTestObjective();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDevCompleteTestObjective(const FString& ObjectiveID);


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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|UI")
	TSubclassOf<class UIdiotsTogetherPhaseHUDWidget> PhaseHUDWidgetClass;

	UPROPERTY()
	class UIdiotsTogetherPhaseHUDWidget* PhaseHUDWidgetInstance;
};
