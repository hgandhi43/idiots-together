#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GroupProjectCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS(config=Game)
class GROUPPROJECT_API AGroupProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* VisibleMesh;

public:
	AGroupProjectCharacter();

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
	virtual void Jump() override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void NotifyControllerChanged() override;
	virtual void PawnClientRestart() override;

	void RegisterMappingContext();

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteract(AActor* TargetActor);

	void PerformInteractionTrace();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Group Project|Interaction")
	float InteractionDistance = 350.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Group Project|Interaction")
	TScriptInterface<class IGroupProjectInteractableInterface> FocusedInteractable;

	AActor* FocusedActor;

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
