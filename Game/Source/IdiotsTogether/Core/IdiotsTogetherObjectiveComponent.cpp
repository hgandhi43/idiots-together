#include "IdiotsTogetherObjectiveComponent.h"
#include "IdiotsTogetherGameModeBase.h"
#include "IdiotsTogetherGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "IdiotsTogether/IdiotsTogether.h"

UIdiotsTogetherObjectiveComponent::UIdiotsTogetherObjectiveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bHasTriggered = false;
	ObjectiveTagToComplete = IdiotsTogetherTags::Objective_Test_Primary;
}

void UIdiotsTogetherObjectiveComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UIdiotsTogetherObjectiveComponent, bHasTriggered);
}

bool UIdiotsTogetherObjectiveComponent::TriggerObjectiveCompletion(APawn* InstigatorPawn)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return false;
	}

	if (bTriggerOnce && bHasTriggered)
	{
		UE_LOG(LogIdiotsTogetherObjective, Verbose, TEXT("Objective component on %s already triggered"), *Owner->GetName());
		return false;
	}

	if (!ObjectiveTagToComplete.IsValid())
	{
		UE_LOG(LogIdiotsTogetherObjective, Warning, TEXT("Objective component on %s has no objective set"), *Owner->GetName());
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	AIdiotsTogetherGameModeBase* GM = World->GetAuthGameMode<AIdiotsTogetherGameModeBase>();
	if (!GM)
	{
		return false;
	}

	bool bSuccess = GM->CompleteObjective(ObjectiveTagToComplete);
	if (bSuccess)
	{
		bHasTriggered = true;
		UE_LOG(LogIdiotsTogetherObjective, Log, TEXT("Objective component on %s completed %s (instigator %s)"),
			*Owner->GetName(), *ObjectiveTagToComplete.ToString(), InstigatorPawn ? *InstigatorPawn->GetName() : TEXT("NULL"));
	}

	return bSuccess;
}
