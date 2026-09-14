#include "IdiotsTogetherObjectiveTrigger.h"
#include "Components/BoxComponent.h"
#include "IdiotsTogether/Core/IdiotsTogetherGameModeBase.h"
#include "IdiotsTogether/Core/IdiotsTogetherCharacter.h"
#include "Net/UnrealNetwork.h"
#include "IdiotsTogether/IdiotsTogether.h"

AIdiotsTogetherObjectiveTrigger::AIdiotsTogetherObjectiveTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));

	TriggerID = TEXT("DefaultTrigger");
	ObjectiveIDToComplete = TEXT("TEST_OBJECTIVE_01");
	bTriggerOnce = true;
	bHasTriggered = false;
}

void AIdiotsTogetherObjectiveTrigger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AIdiotsTogetherObjectiveTrigger, bHasTriggered);
}

void AIdiotsTogetherObjectiveTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AIdiotsTogetherObjectiveTrigger::OnOverlapBegin);
	}
}

void AIdiotsTogetherObjectiveTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bTriggerOnce && bHasTriggered)
	{
		return;
	}

	if (!OtherActor || !OtherActor->IsA<AIdiotsTogetherCharacter>())
	{
		return;
	}

	UE_LOG(LogIdiotsTogether, Warning, TEXT("[SERVER TRIGGER] Player %s entered trigger '%s' (ObjectiveID=%s)"),
		*OtherActor->GetName(), *TriggerID, *ObjectiveIDToComplete);

	if (UWorld* World = GetWorld())
	{
		if (AIdiotsTogetherGameModeBase* GM = World->GetAuthGameMode<AIdiotsTogetherGameModeBase>())
		{
			if (GM->CompleteObjective(ObjectiveIDToComplete))
			{
				bHasTriggered = true;
				UE_LOG(LogIdiotsTogether, Warning, TEXT("[SERVER TRIGGER] Trigger '%s' completed ObjectiveID=%s!"),
					*TriggerID, *ObjectiveIDToComplete);
			}
		}
	}
}
