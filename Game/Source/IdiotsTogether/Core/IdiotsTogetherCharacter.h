#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "IdiotsTogetherTypes.h"
#include "IdiotsTogetherCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UTextRenderComponent;

UCLASS(config=Game)
class IDIOTSTOGETHER_API AIdiotsTogetherCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* VisibleMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* OverheadTextComponent;

public:
	AIdiotsTogetherCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* InteractAction;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(AController* NewController) override;

	/** Re-applies identity shape/label and player colour from the PlayerState. Cosmetic, runs on every machine. */
	UFUNCTION(BlueprintCallable, Category = "Idiots Together|Character")
	void RefreshRepresentation();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void NotifyControllerChanged() override;
	virtual void PawnClientRestart() override;

	void RegisterMappingContext();
	void BindToPlayerState();

	UFUNCTION()
	void OnPlayerStateCharacterTypeChanged(EIdiotsTogetherCharacterType NewType);

	UFUNCTION()
	void OnPlayerStateColorIndexChanged(int32 NewColorIndex);

	/** Placeholder look per identity (label + mesh scale). Replaced by identity data assets in P7.1. */
	UPROPERTY(EditDefaultsOnly, Category = "Idiots Together|Identity")
	TMap<EIdiotsTogetherCharacterType, FIdiotsTogetherIdentityVisual> IdentityVisuals;

	/** Look used while the player has no identity yet. */
	UPROPERTY(EditDefaultsOnly, Category = "Idiots Together|Identity")
	FIdiotsTogetherIdentityVisual UnselectedVisual;

	/** Material vector parameters set to the player colour (names the material lacks are ignored). */
	UPROPERTY(EditDefaultsOnly, Category = "Idiots Together|Identity")
	TArray<FName> MeshColorParameterNames;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteract(AActor* TargetActor);

	void PerformInteractionTrace();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|Interaction")
	float InteractionDistance = 350.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Idiots Together|Interaction")
	TScriptInterface<class IIdiotsTogetherInteractableInterface> FocusedInteractable;

	AActor* FocusedActor;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstance;

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
