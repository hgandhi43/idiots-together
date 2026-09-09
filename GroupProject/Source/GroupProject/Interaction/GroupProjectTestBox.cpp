#include "GroupProjectTestBox.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GroupProject/GroupProject.h"

AGroupProjectTestBox::AGroupProjectTestBox()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	RootComponent = BoxMesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		BoxMesh->SetStaticMesh(CubeMeshAsset.Object);
	}

	BoxMesh->SetCollisionProfileName(TEXT("BlockAll"));
	BoxMesh->SetMobility(EComponentMobility::Movable);

	InteractionPromptText = FText::FromString(TEXT("Press E to Interact"));
	bIsActivated = false;
	bReplicates = true;
}

void AGroupProjectTestBox::BeginPlay()
{
	Super::BeginPlay();
	InitialRotation = GetActorRotation();
	TargetRotation = InitialRotation;
}

void AGroupProjectTestBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Smoothly interpolate rotation to target rotation on interaction
	FRotator CurrentRot = GetActorRotation();
	if (!CurrentRot.Equals(TargetRotation, 0.1f))
	{
		FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRotation, DeltaTime, 5.0f);
		SetActorRotation(NewRot);
	}
}

FText AGroupProjectTestBox::GetInteractionPrompt_Implementation(APawn* InstigatorPawn)
{
	return InteractionPromptText;
}

bool AGroupProjectTestBox::CanInteract_Implementation(APawn* InstigatorPawn)
{
	return true;
}

void AGroupProjectTestBox::Interact_Implementation(APawn* InstigatorPawn)
{
	bIsActivated = !bIsActivated;
	TargetRotation = InitialRotation + FRotator(0.0f, bIsActivated ? 90.0f : 0.0f, 0.0f);

	UE_LOG(LogGroupProject, Log, TEXT("GroupProjectTestBox interacted! New activated state: %s"), bIsActivated ? TEXT("True") : TEXT("False"));
}

void AGroupProjectTestBox::OnBeginFocus_Implementation(APawn* InstigatorPawn)
{
	UE_LOG(LogGroupProject, Log, TEXT("Player began focusing GroupProjectTestBox"));
}

void AGroupProjectTestBox::OnEndFocus_Implementation(APawn* InstigatorPawn)
{
	UE_LOG(LogGroupProject, Log, TEXT("Player ended focusing GroupProjectTestBox"));
}
