#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "IdiotsTogetherTypes.h"
#include "IdiotsTogether/Interaction/IdiotsTogetherInteractionRules.h"
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

	/**
	 * Client -> server: "I want to interact with TargetActor". The target is untrusted: the server
	 * re-checks rate, phase, reach (from its own view of this pawn) and line of sight before acting.
	 * Also used by the listen-server host (runs locally), so every player goes through the same checks.
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteract(AActor* TargetActor);

	/** Server-side validation shared by ServerInteract. Returns false (and logs why) if the request must be ignored. */
	bool CanServerAcceptInteraction(AActor* TargetActor);

	/** True if TargetActor's bounds are within reach of this pawn's eyes and not blocked by other geometry. */
	bool IsTargetReachable(const AActor* TargetActor, float MaxDistance) const;

	/** True if the current round phase allows interacting (no GameState of ours = allowed). */
	bool AreInteractionsAllowedNow() const;

	void PerformInteractionTrace();

	/** Max distance from the pawn's eyes to the closest point of a target's bounds. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idiots Together|Interaction", meta = (ClampMin = "0"))
	float InteractionDistance = 350.0f;

	/** Extra reach the server allows on top of InteractionDistance, to absorb movement latency. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Interaction", meta = (ClampMin = "0"))
	float ServerReachTolerance = 75.0f;

	/** Minimum seconds between accepted ServerInteract requests from this player. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Idiots Together|Interaction", meta = (ClampMin = "0"))
	float MinServerInteractInterval = 0.15f;

	/** Server-only throttle state for ServerInteract. */
	FIdiotsTogetherRateLimiter ServerInteractRateLimiter;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Idiots Together|Interaction")
	TScriptInterface<class IIdiotsTogetherInteractableInterface> FocusedInteractable;

	AActor* FocusedActor;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstance;

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
