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

	/** Client -> server: request an identity (or Random). Single entry point for character selection. */
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Idiots Together|Character")
	void ServerRequestCharacterSelection(EIdiotsTogetherCharacterType Selection);

	/**
	 * Client -> server: dev/cheat command. Only executes in non-Shipping builds and only for the
	 * host (or any client when UIdiotsTogetherSettings::bAllowClientDevCommands is on).
	 * In Shipping, Validate fails, so a client sending it is disconnected.
	 * (UHT does not allow UFUNCTIONs inside #if !UE_BUILD_SHIPPING, so the declaration stays.)
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDevCommand(EIdiotsTogetherDevCommand Command);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	/** Server: may this controller run dev commands? Always false in Shipping. */
	bool IsDevCommandAllowed() const;

	// Dev key handlers. Only bound in non-Shipping builds (see SetupInputComponent).
	void DevSelectBoss();
	void DevSelectNerd();
	void DevSelectMuscle();
	void DevSelectIntern();
	void DevSelectRandom();
	void DevAdvancePhase();
	void DevForceSuccess();
	void DevForceFailure();
	void DevCompleteNextObjective();
	void ToggleDevCursor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|UI")
	TSubclassOf<class UIdiotsTogetherInteractionWidget> InteractionWidgetClass;

	UPROPERTY()
	class UIdiotsTogetherInteractionWidget* InteractionWidgetInstance;

	/** Dev-only widget; never created in Shipping. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|UI")
	TSubclassOf<class UIdiotsTogetherDevCharSelectWidget> DevCharSelectWidgetClass;

	UPROPERTY()
	class UIdiotsTogetherDevCharSelectWidget* DevCharSelectWidgetInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|UI")
	TSubclassOf<class UIdiotsTogetherPhaseHUDWidget> PhaseHUDWidgetClass;

	UPROPERTY()
	class UIdiotsTogetherPhaseHUDWidget* PhaseHUDWidgetInstance;

	/** Viewport Z-orders for the HUD widgets. */
	UPROPERTY(EditDefaultsOnly, Category = "Idiots Together|UI")
	int32 InteractionPromptZOrder = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Idiots Together|UI")
	int32 DevWidgetZOrder = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Idiots Together|UI")
	int32 PhaseHUDZOrder = 2;
};
