#include "IdiotsTogetherGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "IdiotsTogetherSettings.h"
#include "IdiotsTogether/IdiotsTogether.h"

AIdiotsTogetherGameStateBase::AIdiotsTogetherGameStateBase()
{
	CurrentRoundPhase = EIdiotsTogetherRoundPhase::Waiting;
	CurrentRoundIndex = 1;
	RemainingPhaseTime = 0.0f;
}

void AIdiotsTogetherGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIdiotsTogetherGameStateBase, CurrentRoundPhase);
	DOREPLIFETIME(AIdiotsTogetherGameStateBase, CurrentRoundIndex);
	DOREPLIFETIME(AIdiotsTogetherGameStateBase, RemainingPhaseTime);
	DOREPLIFETIME(AIdiotsTogetherGameStateBase, ActiveScenarioTag);
	DOREPLIFETIME(AIdiotsTogetherGameStateBase, ScenarioDisplayName);
	DOREPLIFETIME(AIdiotsTogetherGameStateBase, ScenarioDescription);
	DOREPLIFETIME(AIdiotsTogetherGameStateBase, ObjectiveStates);
}

void AIdiotsTogetherGameStateBase::OnRep_CurrentRoundPhase(EIdiotsTogetherRoundPhase OldPhase)
{
	UE_LOG(LogIdiotsTogetherRound, Verbose, TEXT("[%s] OnRep phase %s -> %s"),
		*GetNetworkRoleString(this), GetRoundPhaseString(OldPhase), GetRoundPhaseString(CurrentRoundPhase));

	OnRoundPhaseChanged.Broadcast(CurrentRoundPhase, OldPhase);
}

void AIdiotsTogetherGameStateBase::OnRep_CurrentRoundIndex()
{
	UE_LOG(LogIdiotsTogetherRound, Verbose, TEXT("OnRep round index %d"), CurrentRoundIndex);
}

void AIdiotsTogetherGameStateBase::OnRep_RemainingPhaseTime()
{
}

void AIdiotsTogetherGameStateBase::OnRep_ActiveScenarioTag()
{
	UE_LOG(LogIdiotsTogetherRound, Verbose, TEXT("OnRep active scenario %s"), *ActiveScenarioTag.ToString());
}

void AIdiotsTogetherGameStateBase::OnRep_ObjectiveStates()
{
	UE_LOG(LogIdiotsTogetherObjective, Verbose, TEXT("[%s] OnRep objectives (%d)"),
		*GetNetworkRoleString(this), ObjectiveStates.Num());

	OnObjectiveStatesChanged.Broadcast();
}

void AIdiotsTogetherGameStateBase::SetCurrentRoundPhase_Server(EIdiotsTogetherRoundPhase NewPhase)
{
	if (HasAuthority() && CurrentRoundPhase != NewPhase)
	{
		EIdiotsTogetherRoundPhase OldPhase = CurrentRoundPhase;
		CurrentRoundPhase = NewPhase;
		OnRoundPhaseChanged.Broadcast(CurrentRoundPhase, OldPhase);
	}
}

bool AIdiotsTogetherGameStateBase::AreInteractionsAllowed() const
{
	return UIdiotsTogetherSettings::Get()->InteractablePhases.Contains(CurrentRoundPhase);
}
