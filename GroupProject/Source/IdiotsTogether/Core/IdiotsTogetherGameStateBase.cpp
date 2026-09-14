#include "IdiotsTogetherGameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "IdiotsTogether/IdiotsTogether.h"

AIdiotsTogetherGameStateBase::AIdiotsTogetherGameStateBase()
{
	CurrentRoundPhase = EIdiotsTogetherRoundPhase::Waiting;
	CurrentRoundIndex = 1;
	RemainingPhaseTime = 0.0f;
	ActiveScenarioID = TEXT("TestScenario");
}

void AIdiotsTogetherGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIdiotsTogetherGameStateBase, CurrentRoundPhase);
	DOREPLIFETIME(AIdiotsTogetherGameStateBase, CurrentRoundIndex);
	DOREPLIFETIME(AIdiotsTogetherGameStateBase, RemainingPhaseTime);
	DOREPLIFETIME(AIdiotsTogetherGameStateBase, ActiveScenarioID);
	DOREPLIFETIME(AIdiotsTogetherGameStateBase, ScenarioDisplayName);
	DOREPLIFETIME(AIdiotsTogetherGameStateBase, ScenarioDescription);
	DOREPLIFETIME(AIdiotsTogetherGameStateBase, ObjectiveStates);
}

void AIdiotsTogetherGameStateBase::OnRep_CurrentRoundPhase(EIdiotsTogetherRoundPhase OldPhase)
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B][%s] OnRep_CurrentRoundPhase: %s -> %s"),
		*GetNetworkRoleString(this), GetRoundPhaseString(OldPhase), GetRoundPhaseString(CurrentRoundPhase));

	OnRoundPhaseChanged.Broadcast(CurrentRoundPhase, OldPhase);
}

void AIdiotsTogetherGameStateBase::OnRep_CurrentRoundIndex()
{
	UE_LOG(LogIdiotsTogether, Log, TEXT("[GAMESTATE REPL] OnRep_CurrentRoundIndex: RoundIndex=%d"), CurrentRoundIndex);
}

void AIdiotsTogetherGameStateBase::OnRep_RemainingPhaseTime()
{
}

void AIdiotsTogetherGameStateBase::OnRep_ActiveScenarioID()
{
	UE_LOG(LogIdiotsTogether, Log, TEXT("[GAMESTATE REPL] OnRep_ActiveScenarioID: ActiveScenarioID=%s"), *ActiveScenarioID);
}

void AIdiotsTogetherGameStateBase::OnRep_ObjectiveStates()
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3D][%s] OnRep_ObjectiveStates: %d objective states updated"),
		*GetNetworkRoleString(this), ObjectiveStates.Num());

	OnObjectiveStatesChanged.Broadcast();
}

void AIdiotsTogetherGameStateBase::SetCurrentRoundPhase_Server(EIdiotsTogetherRoundPhase NewPhase)
{
	if (HasAuthority() && CurrentRoundPhase != NewPhase)
	{
		EIdiotsTogetherRoundPhase OldPhase = CurrentRoundPhase;
		CurrentRoundPhase = NewPhase;
		UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B][%s] GameState phase changed: %s -> %s"),
			*GetNetworkRoleString(this), GetRoundPhaseString(OldPhase), GetRoundPhaseString(CurrentRoundPhase));

		OnRoundPhaseChanged.Broadcast(CurrentRoundPhase, OldPhase);
	}
}
