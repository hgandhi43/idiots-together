#include "GroupProjectCharacter.h"
#include "GroupProjectPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GroupProject/Interaction/GroupProjectInteractableInterface.h"
#include "GroupProject/GroupProject.h"

AGroupProjectCharacter::AGroupProjectCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	VisibleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisibleMesh"));
	VisibleMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		VisibleMesh->SetStaticMesh(CylinderMesh.Object);
		VisibleMesh->SetRelativeLocation(FVector(0.f, 0.f, -10.f));
		VisibleMesh->SetRelativeScale3D(FVector(0.8f, 0.8f, 1.8f));
	}

	bReplicates = true;
	SetReplicatingMovement(true);
	PrimaryActorTick.bCanEverTick = true;
	FocusedActor = nullptr;
}

void AGroupProjectCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] Character BeginPlay: Name=%s, Class=%s, Controller=%s, IsLocallyControlled=%s, Location=%s"),
		*GetName(), *GetClass()->GetName(), Controller ? *Controller->GetName() : TEXT("NULL"),
		IsLocallyControlled() ? TEXT("TRUE") : TEXT("FALSE"), *GetActorLocation().ToString());
	RegisterMappingContext();
}

void AGroupProjectCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] Character NotifyControllerChanged: Controller=%s, IsLocallyControlled=%s"),
		Controller ? *Controller->GetName() : TEXT("NULL"), IsLocallyControlled() ? TEXT("TRUE") : TEXT("FALSE"));
	RegisterMappingContext();
}

void AGroupProjectCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] Character PawnClientRestart: Controller=%s, IsLocallyControlled=%s"),
		Controller ? *Controller->GetName() : TEXT("NULL"), IsLocallyControlled() ? TEXT("TRUE") : TEXT("FALSE"));
	RegisterMappingContext();
}

void AGroupProjectCharacter::RegisterMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
				UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] Added DefaultMappingContext (%s) to EnhancedInputSubsystem for %s"), *DefaultMappingContext->GetName(), *GetName());
			}
			else
			{
				UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] DefaultMappingContext is NULL on %s!"), *GetName());
			}
		}
		else
		{
			UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] EnhancedInputLocalPlayerSubsystem is NULL for %s!"), *PlayerController->GetName());
		}
	}
	else
	{
		UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] RegisterMappingContext skipped because Controller is not APlayerController (Controller=%s)"),
			Controller ? *Controller->GetName() : TEXT("NULL"));
	}
}

void AGroupProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		PerformInteractionTrace();
	}
}

void AGroupProjectCharacter::PerformInteractionTrace()
{
	if (!FollowCamera)
	{
		return;
	}

	float EffectiveTraceDistance = InteractionDistance;
	if (CameraBoom)
	{
		EffectiveTraceDistance += CameraBoom->TargetArmLength;
	}

	FVector StartLoc = FollowCamera->GetComponentLocation();
	FVector EndLoc = StartLoc + (FollowCamera->GetForwardVector() * EffectiveTraceDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECC_Visibility, QueryParams);

	AActor* HitActor = bHit ? HitResult.GetActor() : nullptr;

	if (HitActor && HitActor->GetClass()->ImplementsInterface(UGroupProjectInteractableInterface::StaticClass()))
	{
		if (IGroupProjectInteractableInterface::Execute_CanInteract(HitActor, this))
		{
			if (HitActor != FocusedActor)
			{
				if (FocusedActor)
				{
					IGroupProjectInteractableInterface::Execute_OnEndFocus(FocusedActor, this);
				}

				FocusedActor = HitActor;
				FocusedInteractable = HitActor;
				IGroupProjectInteractableInterface::Execute_OnBeginFocus(FocusedActor, this);
				UE_LOG(LogGroupProject, Warning, TEXT("[INTERACTION] OnBeginFocus called on %s by %s"), *FocusedActor->GetName(), *GetName());
			}

			FText PromptText = IGroupProjectInteractableInterface::Execute_GetInteractionPrompt(FocusedActor, this);

			if (AGroupProjectPlayerController* PC = Cast<AGroupProjectPlayerController>(Controller))
			{
				PC->ShowInteractionPrompt(PromptText);
				UE_LOG(LogGroupProject, Warning, TEXT("[INTERACTION] ShowInteractionPrompt called with prompt: '%s'"), *PromptText.ToString());
			}
			return;
		}
	}

	if (FocusedActor)
	{
		IGroupProjectInteractableInterface::Execute_OnEndFocus(FocusedActor, this);
		FocusedActor = nullptr;
		FocusedInteractable = nullptr;

		if (AGroupProjectPlayerController* PC = Cast<AGroupProjectPlayerController>(Controller))
		{
			PC->HideInteractionPrompt();
		}
	}
}

void AGroupProjectCharacter::Jump()
{
	Super::Jump();
	UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] IA_Jump fired!"));
}

void AGroupProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] SetupPlayerInputComponent called on %s!"), *GetName());

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGroupProjectCharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
			UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] Bound JumpAction (%s)"), *JumpAction->GetName());
		}

		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGroupProjectCharacter::Move);
			UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] Bound MoveAction (%s)"), *MoveAction->GetName());
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGroupProjectCharacter::Look);
			UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] Bound LookAction (%s)"), *LookAction->GetName());
		}

		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AGroupProjectCharacter::Interact);
			UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] Bound InteractAction (%s)"), *InteractAction->GetName());
		}
	}
}

void AGroupProjectCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		const FVector FinalForward = ForwardDirection * MovementVector.Y;
		const FVector FinalRight = RightDirection * MovementVector.X;
		const FVector TotalInputVector = FinalForward + FinalRight;

		UE_LOG(LogGroupProject, Warning, TEXT("[MOVEMENT DIAGNOSTIC] IA_Move: Input2D=(X=%.2f, Y=%.2f) | ControllerYaw=%.2f | ForwardVec=(X=%.2f, Y=%.2f, Z=%.2f) | RightVec=(X=%.2f, Y=%.2f, Z=%.2f) | FinalMove=(X=%.2f, Y=%.2f, Z=%.2f)"),
			MovementVector.X, MovementVector.Y, Rotation.Yaw,
			ForwardDirection.X, ForwardDirection.Y, ForwardDirection.Z,
			RightDirection.X, RightDirection.Y, RightDirection.Z,
			TotalInputVector.X, TotalInputVector.Y, TotalInputVector.Z);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AGroupProjectCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] IA_Look fired! Vector=(X=%.2f, Y=%.2f), Controller=%s"),
		LookAxisVector.X, LookAxisVector.Y, Controller ? *Controller->GetName() : TEXT("NULL"));

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGroupProjectCharacter::Interact()
{
	UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] IA_Interact fired! FocusedActor=%s, HasAuthority=%s"),
		FocusedActor ? *FocusedActor->GetName() : TEXT("NONE"),
		HasAuthority() ? TEXT("TRUE") : TEXT("FALSE"));

	if (FocusedActor && FocusedActor->GetClass()->ImplementsInterface(UGroupProjectInteractableInterface::StaticClass()))
	{
		if (IGroupProjectInteractableInterface::Execute_CanInteract(FocusedActor, this))
		{
			if (HasAuthority())
			{
				IGroupProjectInteractableInterface::Execute_Interact(FocusedActor, this);
			}
			else
			{
				ServerInteract(FocusedActor);
			}
		}
	}
}

void AGroupProjectCharacter::ServerInteract_Implementation(AActor* TargetActor)
{
	UE_LOG(LogGroupProject, Warning, TEXT("[SERVER] ServerInteract received from client %s for target %s"),
		*GetName(), TargetActor ? *TargetActor->GetName() : TEXT("NULL"));

	if (TargetActor && TargetActor->GetClass()->ImplementsInterface(UGroupProjectInteractableInterface::StaticClass()))
	{
		float DistSq = FVector::DistSquared(GetActorLocation(), TargetActor->GetActorLocation());
		if (DistSq <= FMath::Square(InteractionDistance + 200.0f))
		{
			if (IGroupProjectInteractableInterface::Execute_CanInteract(TargetActor, this))
			{
				IGroupProjectInteractableInterface::Execute_Interact(TargetActor, this);
			}
		}
	}
}

bool AGroupProjectCharacter::ServerInteract_Validate(AActor* TargetActor)
{
	return true;
}
