#include "IdiotsTogetherTestBox.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/UnrealNetwork.h"
#include "IdiotsTogether/Core/IdiotsTogetherGameModeBase.h"
#include "IdiotsTogether/IdiotsTogether.h"

AIdiotsTogetherTestBox::AIdiotsTogetherTestBox()
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
	ObjectiveIDToComplete = TEXT("TEST_OBJECTIVE_01");
	bIsActivated = false;
	bReplicates = true;
}

void AIdiotsTogetherTestBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIdiotsTogetherTestBox, bIsActivated);
	DOREPLIFETIME(AIdiotsTogetherTestBox, TargetRotation);
}

void AIdiotsTogetherTestBox::BeginPlay()
{
	Super::BeginPlay();
	InitialRotation = GetActorRotation();
	TargetRotation = InitialRotation;
}

void AIdiotsTogetherTestBox::Tick(float DeltaTime)
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

FText AIdiotsTogetherTestBox::GetInteractionPrompt_Implementation(APawn* InstigatorPawn)
{
	return InteractionPromptText;
}

bool AIdiotsTogetherTestBox::CanInteract_Implementation(APawn* InstigatorPawn)
{
	return true;
}

void AIdiotsTogetherTestBox::Interact_Implementation(APawn* InstigatorPawn)
{
	if (HasAuthority())
	{
		bIsActivated = !bIsActivated;
		TargetRotation = InitialRotation + FRotator(0.0f, bIsActivated ? 90.0f : 0.0f, 0.0f);

		UE_LOG(LogIdiotsTogetherInteraction, Verbose, TEXT("Test box interacted by %s (activated=%s, target rotation %s)"),
			InstigatorPawn ? *InstigatorPawn->GetName() : TEXT("NULL"),
			bIsActivated ? TEXT("True") : TEXT("False"),
			*TargetRotation.ToString());

		if (!ObjectiveIDToComplete.IsEmpty())
		{
			if (UWorld* World = GetWorld())
			{
				if (AIdiotsTogetherGameModeBase* GM = World->GetAuthGameMode<AIdiotsTogetherGameModeBase>())
				{
					GM->CompleteObjective(ObjectiveIDToComplete);
				}
			}
		}
	}
}

void AIdiotsTogetherTestBox::OnRep_TargetRotation()
{
	UE_LOG(LogIdiotsTogetherInteraction, VeryVerbose, TEXT("Test box target rotation replicated: %s"), *TargetRotation.ToString());
}

void AIdiotsTogetherTestBox::OnBeginFocus_Implementation(APawn* InstigatorPawn)
{
	UE_LOG(LogIdiotsTogetherInteraction, VeryVerbose, TEXT("Began focusing %s"), *GetName());
}

void AIdiotsTogetherTestBox::OnEndFocus_Implementation(APawn* InstigatorPawn)
{
	UE_LOG(LogIdiotsTogetherInteraction, VeryVerbose, TEXT("Ended focusing %s"), *GetName());
}
