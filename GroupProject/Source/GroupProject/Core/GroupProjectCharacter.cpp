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

	bReplicates = true;
	SetReplicatingMovement(true);
	PrimaryActorTick.bCanEverTick = true;
	FocusedActor = nullptr;
}

void AGroupProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
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

	FVector StartLoc = FollowCamera->GetComponentLocation();
	FVector EndLoc = StartLoc + (FollowCamera->GetForwardVector() * InteractionDistance);

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
			}

			FText PromptText = IGroupProjectInteractableInterface::Execute_GetInteractionPrompt(FocusedActor, this);

			if (AGroupProjectPlayerController* PC = Cast<AGroupProjectPlayerController>(Controller))
			{
				PC->ShowInteractionPrompt(PromptText);
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

void AGroupProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}

		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGroupProjectCharacter::Move);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGroupProjectCharacter::Look);
		}

		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AGroupProjectCharacter::Interact);
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

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AGroupProjectCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGroupProjectCharacter::Interact()
{
	if (FocusedActor && FocusedActor->GetClass()->ImplementsInterface(UGroupProjectInteractableInterface::StaticClass()))
	{
		if (IGroupProjectInteractableInterface::Execute_CanInteract(FocusedActor, this))
		{
			IGroupProjectInteractableInterface::Execute_Interact(FocusedActor, this);
		}
	}
}
