#include "IdiotsTogetherCharacter.h"
#include "IdiotsTogetherPlayerController.h"
#include "IdiotsTogetherPlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "IdiotsTogether/Interaction/IdiotsTogetherInteractableInterface.h"
#include "IdiotsTogether/IdiotsTogether.h"

AIdiotsTogetherCharacter::AIdiotsTogetherCharacter()
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

	OverheadTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("OverheadTextComponent"));
	OverheadTextComponent->SetupAttachment(RootComponent);
	OverheadTextComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 130.0f));
	OverheadTextComponent->SetHorizontalAlignment(EHTA_Center);
	OverheadTextComponent->SetWorldSize(32.0f);
	OverheadTextComponent->SetText(FText::FromString(TEXT("[UNSELECTED]")));

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC(TEXT("/Game/Input/IMC_Default.IMC_Default"));
	if (IMC.Succeeded())
	{
		DefaultMappingContext = IMC.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IAMove(TEXT("/Game/Input/IA_Move.IA_Move"));
	if (IAMove.Succeeded())
	{
		MoveAction = IAMove.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IALook(TEXT("/Game/Input/IA_Look.IA_Look"));
	if (IALook.Succeeded())
	{
		LookAction = IALook.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IAJump(TEXT("/Game/Input/IA_Jump.IA_Jump"));
	if (IAJump.Succeeded())
	{
		JumpAction = IAJump.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IAInteract(TEXT("/Game/Input/IA_Interact.IA_Interact"));
	if (IAInteract.Succeeded())
	{
		InteractAction = IAInteract.Object;
	}

	bReplicates = true;
	SetReplicatingMovement(true);
	PrimaryActorTick.bCanEverTick = true;
	FocusedActor = nullptr;
	DynamicMaterialInstance = nullptr;
}

void AIdiotsTogetherCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] Character BeginPlay: Name=%s, Class=%s, Controller=%s, IsLocallyControlled=%s, Location=%s"),
		*GetName(), *GetClass()->GetName(), Controller ? *Controller->GetName() : TEXT("NULL"),
		IsLocallyControlled() ? TEXT("TRUE") : TEXT("FALSE"), *GetActorLocation().ToString());
	RegisterMappingContext();
	BindToPlayerState();
}

void AIdiotsTogetherCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] Character OnRep_PlayerState on %s"), *GetName());
	BindToPlayerState();
}

void AIdiotsTogetherCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] Character PossessedBy on %s by %s"), *GetName(), NewController ? *NewController->GetName() : TEXT("NULL"));
	BindToPlayerState();
}

void AIdiotsTogetherCharacter::BindToPlayerState()
{
	if (AIdiotsTogetherPlayerState* PS = GetPlayerState<AIdiotsTogetherPlayerState>())
	{
		PS->OnCharacterTypeChanged.RemoveDynamic(this, &AIdiotsTogetherCharacter::OnPlayerStateCharacterTypeChanged);
		PS->OnCharacterTypeChanged.AddDynamic(this, &AIdiotsTogetherCharacter::OnPlayerStateCharacterTypeChanged);

		UpdateCharacterRepresentation(PS->GetCharacterType());
		UE_LOG(LogIdiotsTogether, Warning, TEXT("[BINDING] Bound %s to PlayerState %s with CharacterType=%d"),
			*GetName(), *PS->GetName(), static_cast<int32>(PS->GetCharacterType()));
	}
}

void AIdiotsTogetherCharacter::OnPlayerStateCharacterTypeChanged(EIdiotsTogetherCharacterType NewType)
{
	UpdateCharacterRepresentation(NewType);
}

void AIdiotsTogetherCharacter::UpdateCharacterRepresentation(EIdiotsTogetherCharacterType NewType)
{
	if (!VisibleMesh)
	{
		return;
	}

	if (!DynamicMaterialInstance && VisibleMesh->GetMaterial(0))
	{
		DynamicMaterialInstance = VisibleMesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	FLinearColor CharColor = FLinearColor(0.4f, 0.4f, 0.4f, 1.0f);
	FVector MeshScale = FVector(0.8f, 0.8f, 1.8f);
	FString DisplayName = TEXT("[UNSELECTED]");

	switch (NewType)
	{
	case EIdiotsTogetherCharacterType::Boss:
		CharColor = FLinearColor(1.0f, 0.8f, 0.0f, 1.0f); // Confident Gold
		MeshScale = FVector(0.9f, 0.9f, 1.85f);
		DisplayName = TEXT("[BOSS]");
		break;
	case EIdiotsTogetherCharacterType::Nerd:
		CharColor = FLinearColor(0.0f, 0.75f, 1.0f, 1.0f); // Smart Cyan / Blue
		MeshScale = FVector(0.75f, 0.75f, 1.95f);
		DisplayName = TEXT("[NERD]");
		break;
	case EIdiotsTogetherCharacterType::Muscle:
		CharColor = FLinearColor(1.0f, 0.15f, 0.15f, 1.0f); // Strong Red
		MeshScale = FVector(1.25f, 1.25f, 1.7f);
		DisplayName = TEXT("[MUSCLE]");
		break;
	case EIdiotsTogetherCharacterType::Intern:
		CharColor = FLinearColor(0.2f, 0.85f, 0.2f, 1.0f); // Young/Inexperienced Green
		MeshScale = FVector(0.75f, 0.75f, 1.45f);
		DisplayName = TEXT("[INTERN]");
		break;
	default:
		break;
	}

	VisibleMesh->SetRelativeScale3D(MeshScale);

	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("Color"), CharColor);
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("BaseColor"), CharColor);
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("TintColor"), CharColor);
	}

	if (OverheadTextComponent)
	{
		OverheadTextComponent->SetText(FText::FromString(DisplayName));
		OverheadTextComponent->SetTextRenderColor(CharColor.ToFColor(true));
	}

	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3A REPRESENTATION UPDATE] M3A REPRESENTATION UPDATE: Character=%s, Type=%d (%s)"),
		*GetName(), static_cast<int32>(NewType), *DisplayName);
}

void AIdiotsTogetherCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] Character NotifyControllerChanged: Controller=%s, IsLocallyControlled=%s"),
		Controller ? *Controller->GetName() : TEXT("NULL"), IsLocallyControlled() ? TEXT("TRUE") : TEXT("FALSE"));
	RegisterMappingContext();
}

void AIdiotsTogetherCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] Character PawnClientRestart: Controller=%s, IsLocallyControlled=%s"),
		Controller ? *Controller->GetName() : TEXT("NULL"), IsLocallyControlled() ? TEXT("TRUE") : TEXT("FALSE"));
	RegisterMappingContext();
}

void AIdiotsTogetherCharacter::RegisterMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
				UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] Added DefaultMappingContext (%s) to EnhancedInputSubsystem for %s"), *DefaultMappingContext->GetName(), *GetName());
			}
			else
			{
				UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] DefaultMappingContext is NULL on %s!"), *GetName());
			}
		}
		else
		{
			UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] EnhancedInputLocalPlayerSubsystem is NULL for %s!"), *PlayerController->GetName());
		}
	}
	else
	{
		UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] RegisterMappingContext skipped because Controller is not APlayerController (Controller=%s)"),
			Controller ? *Controller->GetName() : TEXT("NULL"));
	}
}

void AIdiotsTogetherCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OverheadTextComponent && GetWorld())
	{
		if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
		{
			if (APlayerCameraManager* CameraManager = PC->PlayerCameraManager)
			{
				FVector CameraLoc = CameraManager->GetCameraLocation();
				FVector TextLoc = OverheadTextComponent->GetComponentLocation();
				FRotator LookAtRot = (CameraLoc - TextLoc).Rotation();
				LookAtRot.Pitch = 0.0f;
				LookAtRot.Roll = 0.0f;
				OverheadTextComponent->SetWorldRotation(LookAtRot);
			}
		}
	}

	if (IsLocallyControlled())
	{
		PerformInteractionTrace();
	}
}

void AIdiotsTogetherCharacter::PerformInteractionTrace()
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

	if (HitActor && HitActor->GetClass()->ImplementsInterface(UIdiotsTogetherInteractableInterface::StaticClass()))
	{
		if (IIdiotsTogetherInteractableInterface::Execute_CanInteract(HitActor, this))
		{
			if (HitActor != FocusedActor)
			{
				if (FocusedActor)
				{
					IIdiotsTogetherInteractableInterface::Execute_OnEndFocus(FocusedActor, this);
				}

				FocusedActor = HitActor;
				FocusedInteractable = HitActor;
				IIdiotsTogetherInteractableInterface::Execute_OnBeginFocus(FocusedActor, this);
				UE_LOG(LogIdiotsTogether, Warning, TEXT("[INTERACTION] OnBeginFocus called on %s by %s"), *FocusedActor->GetName(), *GetName());
			}

			FText PromptText = IIdiotsTogetherInteractableInterface::Execute_GetInteractionPrompt(FocusedActor, this);

			if (AIdiotsTogetherPlayerController* PC = Cast<AIdiotsTogetherPlayerController>(Controller))
			{
				PC->ShowInteractionPrompt(PromptText);
				UE_LOG(LogIdiotsTogether, Warning, TEXT("[INTERACTION] ShowInteractionPrompt called with prompt: '%s'"), *PromptText.ToString());
			}
			return;
		}
	}

	if (FocusedActor)
	{
		IIdiotsTogetherInteractableInterface::Execute_OnEndFocus(FocusedActor, this);
		FocusedActor = nullptr;
		FocusedInteractable = nullptr;

		if (AIdiotsTogetherPlayerController* PC = Cast<AIdiotsTogetherPlayerController>(Controller))
		{
			PC->HideInteractionPrompt();
		}
	}
}

void AIdiotsTogetherCharacter::Jump()
{
	Super::Jump();
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] IA_Jump fired!"));
}

void AIdiotsTogetherCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] SetupPlayerInputComponent called on %s!"), *GetName());

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AIdiotsTogetherCharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
			UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] Bound JumpAction (%s)"), *JumpAction->GetName());
		}

		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIdiotsTogetherCharacter::Move);
			UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] Bound MoveAction (%s)"), *MoveAction->GetName());
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AIdiotsTogetherCharacter::Look);
			UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] Bound LookAction (%s)"), *LookAction->GetName());
		}

		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AIdiotsTogetherCharacter::Interact);
			UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] Bound InteractAction (%s)"), *InteractAction->GetName());
		}
	}
}

void AIdiotsTogetherCharacter::Move(const FInputActionValue& Value)
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

		UE_LOG(LogIdiotsTogether, Warning, TEXT("[MOVEMENT DIAGNOSTIC] IA_Move: Input2D=(X=%.2f, Y=%.2f) | ControllerYaw=%.2f | ForwardVec=(X=%.2f, Y=%.2f, Z=%.2f) | RightVec=(X=%.2f, Y=%.2f, Z=%.2f) | FinalMove=(X=%.2f, Y=%.2f, Z=%.2f)"),
			MovementVector.X, MovementVector.Y, Rotation.Yaw,
			ForwardDirection.X, ForwardDirection.Y, ForwardDirection.Z,
			RightDirection.X, RightDirection.Y, RightDirection.Z,
			TotalInputVector.X, TotalInputVector.Y, TotalInputVector.Z);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AIdiotsTogetherCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] IA_Look fired! Vector=(X=%.2f, Y=%.2f), Controller=%s"),
		LookAxisVector.X, LookAxisVector.Y, Controller ? *Controller->GetName() : TEXT("NULL"));

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AIdiotsTogetherCharacter::Interact()
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] IA_Interact fired! FocusedActor=%s, HasAuthority=%s"),
		FocusedActor ? *FocusedActor->GetName() : TEXT("NONE"),
		HasAuthority() ? TEXT("TRUE") : TEXT("FALSE"));

	if (FocusedActor && FocusedActor->GetClass()->ImplementsInterface(UIdiotsTogetherInteractableInterface::StaticClass()))
	{
		if (IIdiotsTogetherInteractableInterface::Execute_CanInteract(FocusedActor, this))
		{
			if (HasAuthority())
			{
				IIdiotsTogetherInteractableInterface::Execute_Interact(FocusedActor, this);
			}
			else
			{
				ServerInteract(FocusedActor);
			}
		}
	}
}

void AIdiotsTogetherCharacter::ServerInteract_Implementation(AActor* TargetActor)
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[SERVER] ServerInteract received from client %s for target %s"),
		*GetName(), TargetActor ? *TargetActor->GetName() : TEXT("NULL"));

	if (TargetActor && TargetActor->GetClass()->ImplementsInterface(UIdiotsTogetherInteractableInterface::StaticClass()))
	{
		float DistSq = FVector::DistSquared(GetActorLocation(), TargetActor->GetActorLocation());
		if (DistSq <= FMath::Square(InteractionDistance + 200.0f))
		{
			if (IIdiotsTogetherInteractableInterface::Execute_CanInteract(TargetActor, this))
			{
				IIdiotsTogetherInteractableInterface::Execute_Interact(TargetActor, this);
			}
		}
	}
}

bool AIdiotsTogetherCharacter::ServerInteract_Validate(AActor* TargetActor)
{
	return true;
}
