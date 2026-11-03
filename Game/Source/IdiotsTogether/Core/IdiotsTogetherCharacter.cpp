#include "IdiotsTogetherCharacter.h"
#include "IdiotsTogetherPlayerController.h"
#include "IdiotsTogetherPlayerState.h"
#include "IdiotsTogetherSettings.h"
#include "IdiotsTogetherGameStateBase.h"
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
	OverheadTextComponent->SetText(NSLOCTEXT("IdiotsTogether", "IdentityUnselected", "[UNSELECTED]"));

	auto MakeVisual = [](const FText& Label, const FVector& Scale)
	{
		FIdiotsTogetherIdentityVisual Visual;
		Visual.Label = Label;
		Visual.MeshScale = Scale;
		return Visual;
	};
	UnselectedVisual = MakeVisual(NSLOCTEXT("IdiotsTogether", "IdentityUnselected", "[UNSELECTED]"), FVector(0.8f, 0.8f, 1.8f));
	IdentityVisuals.Add(EIdiotsTogetherCharacterType::Boss,   MakeVisual(NSLOCTEXT("IdiotsTogether", "IdentityBoss", "[BOSS]"),     FVector(0.9f, 0.9f, 1.85f)));
	IdentityVisuals.Add(EIdiotsTogetherCharacterType::Nerd,   MakeVisual(NSLOCTEXT("IdiotsTogether", "IdentityNerd", "[NERD]"),     FVector(0.75f, 0.75f, 1.95f)));
	IdentityVisuals.Add(EIdiotsTogetherCharacterType::Muscle, MakeVisual(NSLOCTEXT("IdiotsTogether", "IdentityMuscle", "[MUSCLE]"), FVector(1.25f, 1.25f, 1.7f)));
	IdentityVisuals.Add(EIdiotsTogetherCharacterType::Intern, MakeVisual(NSLOCTEXT("IdiotsTogether", "IdentityIntern", "[INTERN]"), FVector(0.75f, 0.75f, 1.45f)));

	MeshColorParameterNames = { TEXT("Color"), TEXT("BaseColor"), TEXT("TintColor") };

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
	UE_LOG(LogIdiotsTogetherPlayer, Verbose, TEXT("[%s] Character BeginPlay: %s (Controller=%s, Local=%d)"),
		*GetNetworkRoleString(this), *GetName(), Controller ? *Controller->GetName() : TEXT("None"), IsLocallyControlled());
	RegisterMappingContext();
	BindToPlayerState();
}

void AIdiotsTogetherCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	BindToPlayerState();
}

void AIdiotsTogetherCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	BindToPlayerState();
}

void AIdiotsTogetherCharacter::BindToPlayerState()
{
	if (AIdiotsTogetherPlayerState* PS = GetPlayerState<AIdiotsTogetherPlayerState>())
	{
		PS->OnCharacterTypeChanged.RemoveDynamic(this, &AIdiotsTogetherCharacter::OnPlayerStateCharacterTypeChanged);
		PS->OnCharacterTypeChanged.AddDynamic(this, &AIdiotsTogetherCharacter::OnPlayerStateCharacterTypeChanged);
		PS->OnPlayerColorIndexChanged.RemoveDynamic(this, &AIdiotsTogetherCharacter::OnPlayerStateColorIndexChanged);
		PS->OnPlayerColorIndexChanged.AddDynamic(this, &AIdiotsTogetherCharacter::OnPlayerStateColorIndexChanged);

		RefreshRepresentation();
		UE_LOG(LogIdiotsTogetherPlayer, Verbose, TEXT("Bound %s to PlayerState %s (CharacterType=%d, Colour=%d)"),
			*GetName(), *PS->GetPlayerName(), static_cast<int32>(PS->GetCharacterType()), PS->GetPlayerColorIndex());
	}
}

void AIdiotsTogetherCharacter::OnPlayerStateCharacterTypeChanged(EIdiotsTogetherCharacterType NewType)
{
	RefreshRepresentation();
}

void AIdiotsTogetherCharacter::OnPlayerStateColorIndexChanged(int32 NewColorIndex)
{
	RefreshRepresentation();
}

void AIdiotsTogetherCharacter::RefreshRepresentation()
{
	if (!VisibleMesh)
	{
		return;
	}

	const AIdiotsTogetherPlayerState* PS = GetPlayerState<AIdiotsTogetherPlayerState>();
	const EIdiotsTogetherCharacterType Identity = PS ? PS->GetCharacterType() : EIdiotsTogetherCharacterType::None;
	const FLinearColor PlayerColor = PS ? PS->GetPlayerColor() : UIdiotsTogetherSettings::Get()->UnassignedPlayerColor;

	const FIdiotsTogetherIdentityVisual* FoundVisual = IdentityVisuals.Find(Identity);
	const FIdiotsTogetherIdentityVisual& Visual = FoundVisual ? *FoundVisual : UnselectedVisual;

	if (!DynamicMaterialInstance && VisibleMesh->GetMaterial(0))
	{
		DynamicMaterialInstance = VisibleMesh->CreateAndSetMaterialInstanceDynamic(0);
	}

	VisibleMesh->SetRelativeScale3D(Visual.MeshScale);

	if (DynamicMaterialInstance)
	{
		for (const FName& ParamName : MeshColorParameterNames)
		{
			DynamicMaterialInstance->SetVectorParameterValue(ParamName, PlayerColor);
		}
	}

	if (OverheadTextComponent)
	{
		OverheadTextComponent->SetText(Visual.Label);
		OverheadTextComponent->SetTextRenderColor(PlayerColor.ToFColor(true));
	}

	UE_LOG(LogIdiotsTogetherPlayer, Verbose, TEXT("Representation updated: %s -> %s"), *GetName(), *Visual.Label.ToString());
}

void AIdiotsTogetherCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	RegisterMappingContext();
}

void AIdiotsTogetherCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	RegisterMappingContext();
}

void AIdiotsTogetherCharacter::RegisterMappingContext()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!PlayerController)
	{
		return;
	}

	// Remote players' controllers on the server have no LocalPlayer, so no subsystem: that is expected.
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!Subsystem)
	{
		return;
	}

	if (DefaultMappingContext)
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
		UE_LOG(LogIdiotsTogetherPlayer, Verbose, TEXT("Added mapping context %s for %s"), *DefaultMappingContext->GetName(), *GetName());
	}
	else
	{
		UE_LOG(LogIdiotsTogetherPlayer, Warning, TEXT("DefaultMappingContext is not set on %s; player input will not work."), *GetName());
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

	// Mirror the server's rules so the prompt never promises an interaction the server will refuse.
	if (HitActor && (!AreInteractionsAllowedNow() || !IsTargetReachable(HitActor, InteractionDistance)))
	{
		HitActor = nullptr;
	}

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
				UE_LOG(LogIdiotsTogetherInteraction, Verbose, TEXT("%s began focusing %s"), *GetName(), *FocusedActor->GetName());
			}

			FText PromptText = IIdiotsTogetherInteractableInterface::Execute_GetInteractionPrompt(FocusedActor, this);

			if (AIdiotsTogetherPlayerController* PC = Cast<AIdiotsTogetherPlayerController>(Controller))
			{
				PC->ShowInteractionPrompt(PromptText);
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

void AIdiotsTogetherCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogIdiotsTogetherPlayer, Warning, TEXT("%s: input component is not an EnhancedInputComponent; no actions bound."), *GetName());
		return;
	}

	if (JumpAction)
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}

	if (MoveAction)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AIdiotsTogetherCharacter::Move);
	}

	if (LookAction)
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AIdiotsTogetherCharacter::Look);
	}

	if (InteractAction)
	{
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AIdiotsTogetherCharacter::Interact);
	}
}

void AIdiotsTogetherCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

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

void AIdiotsTogetherCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AIdiotsTogetherCharacter::Interact()
{
	UE_LOG(LogIdiotsTogetherInteraction, Verbose, TEXT("%s pressed Interact (Focused=%s)"),
		*GetName(), FocusedActor ? *FocusedActor->GetName() : TEXT("None"));

	if (FocusedActor)
	{
		// Host and clients take the same path: on the listen server this runs locally,
		// so the host is subject to the same server checks as everyone else.
		ServerInteract(FocusedActor);
	}
}

bool AIdiotsTogetherCharacter::AreInteractionsAllowedNow() const
{
	const UWorld* World = GetWorld();
	const AIdiotsTogetherGameStateBase* GS = World ? World->GetGameState<AIdiotsTogetherGameStateBase>() : nullptr;
	return !GS || GS->AreInteractionsAllowed();
}

bool AIdiotsTogetherCharacter::IsTargetReachable(const AActor* TargetActor, float MaxDistance) const
{
	if (!TargetActor)
	{
		return false;
	}

	FBox TargetBounds = TargetActor->GetComponentsBoundingBox();
	if (!TargetBounds.IsValid)
	{
		const FVector Origin = TargetActor->GetActorLocation();
		TargetBounds = FBox(Origin, Origin);
	}

	const FVector ViewLocation = GetPawnViewLocation();
	if (!IdiotsTogetherInteractionRules::IsWithinReach(ViewLocation, TargetBounds, MaxDistance))
	{
		return false;
	}

	// Line of sight: nothing other than the target may sit between the eyes and the target.
	const FVector AimPoint = IdiotsTogetherInteractionRules::GetClosestPoint(ViewLocation, TargetBounds);
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	const bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, AimPoint, ECC_Visibility, Params);
	return !bBlocked || Hit.GetActor() == TargetActor;
}

bool AIdiotsTogetherCharacter::CanServerAcceptInteraction(AActor* TargetActor)
{
	const double Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	if (!ServerInteractRateLimiter.TryAccept(Now, MinServerInteractInterval))
	{
		UE_LOG(LogIdiotsTogetherInteraction, Verbose, TEXT("ServerInteract from %s throttled"), *GetName());
		return false;
	}

	if (!IsValid(TargetActor) || TargetActor->GetWorld() != GetWorld()
		|| !TargetActor->GetClass()->ImplementsInterface(UIdiotsTogetherInteractableInterface::StaticClass()))
	{
		UE_LOG(LogIdiotsTogetherInteraction, Verbose, TEXT("ServerInteract from %s rejected: invalid target"), *GetName());
		return false;
	}

	if (!AreInteractionsAllowedNow())
	{
		UE_LOG(LogIdiotsTogetherInteraction, Verbose, TEXT("ServerInteract from %s rejected: phase does not allow interaction"), *GetName());
		return false;
	}

	if (!IsTargetReachable(TargetActor, InteractionDistance + ServerReachTolerance))
	{
		UE_LOG(LogIdiotsTogetherInteraction, Log, TEXT("ServerInteract from %s rejected: %s out of reach or not visible"),
			*GetName(), *TargetActor->GetName());
		return false;
	}

	return IIdiotsTogetherInteractableInterface::Execute_CanInteract(TargetActor, this);
}

void AIdiotsTogetherCharacter::ServerInteract_Implementation(AActor* TargetActor)
{
	if (!CanServerAcceptInteraction(TargetActor))
	{
		return;
	}

	UE_LOG(LogIdiotsTogetherInteraction, Verbose, TEXT("%s interacts with %s"), *GetName(), *TargetActor->GetName());
	IIdiotsTogetherInteractableInterface::Execute_Interact(TargetActor, this);
}

bool AIdiotsTogetherCharacter::ServerInteract_Validate(AActor* TargetActor)
{
	// A null/stale target is normal under latency (actor destroyed in flight), so never kick for it.
	// All real checks happen in CanServerAcceptInteraction.
	return true;
}
