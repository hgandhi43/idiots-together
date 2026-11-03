#include "IdiotsTogetherGameModeBase.h"
#include "IdiotsTogetherPlayerController.h"
#include "IdiotsTogetherGameStateBase.h"
#include "IdiotsTogetherPlayerState.h"
#include "IdiotsTogetherCharacter.h"
#include "IdiotsTogetherScenarioDefinition.h"
#include "IdiotsTogether/IdiotsTogether.h"
#include "TimerManager.h"

AIdiotsTogetherGameModeBase::AIdiotsTogetherGameModeBase()
{
	PlayerControllerClass = AIdiotsTogetherPlayerController::StaticClass();
	static ConstructorHelpers::FClassFinder<APlayerController> PCClass(TEXT("/Game/Core/Game/BP_IdiotsTogetherPlayerController.BP_IdiotsTogetherPlayerController_C"));
	if (PCClass.Succeeded())
	{
		PlayerControllerClass = PCClass.Class;
	}

	DefaultPawnClass = AIdiotsTogetherCharacter::StaticClass();
	static ConstructorHelpers::FClassFinder<APawn> PawnClass(TEXT("/Game/Core/Characters/BP_IdiotsTogetherCharacter.BP_IdiotsTogetherCharacter_C"));
	if (PawnClass.Succeeded())
	{
		DefaultPawnClass = PawnClass.Class;
	}

	GameStateClass = AIdiotsTogetherGameStateBase::StaticClass();
	PlayerStateClass = AIdiotsTogetherPlayerState::StaticClass();

	ActiveScenarioDefinition = nullptr;
}

void AIdiotsTogetherGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	if (!ActiveScenarioDefinition)
	{
		ActiveScenarioDefinition = NewObject<UIdiotsTogetherScenarioDefinition>(this, UIdiotsTogetherScenarioDefinition::StaticClass());
		UE_LOG(LogIdiotsTogetherRound, Warning, TEXT("No ActiveScenarioDefinition set on %s; using built-in test scenario."), *GetClass()->GetName());
	}

	StartScenarioLifecycle();
}

void AIdiotsTogetherGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogIdiotsTogetherPlayer, Log, TEXT("Player joined: %s"), *NewPlayer->GetName());
}

void AIdiotsTogetherGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (Exiting)
	{
		UE_LOG(LogIdiotsTogetherPlayer, Log, TEXT("Player left: %s"), *Exiting->GetName());
	}
}

void AIdiotsTogetherGameModeBase::StartScenarioLifecycle()
{
	if (!HasAuthority())
	{
		return;
	}

	AIdiotsTogetherGameStateBase* GS = GetGameState<AIdiotsTogetherGameStateBase>();
	if (GS && ActiveScenarioDefinition)
	{
		GS->ActiveScenarioID = ActiveScenarioDefinition->ScenarioID;
		GS->ScenarioDisplayName = ActiveScenarioDefinition->DisplayName;
		GS->ScenarioDescription = ActiveScenarioDefinition->Description;
		GS->CurrentRoundIndex = 1;
		UE_LOG(LogIdiotsTogetherRound, Log, TEXT("Scenario lifecycle started: %s ('%s'), %d rounds"),
			*GS->ActiveScenarioID, *GS->ScenarioDisplayName.ToString(), ActiveScenarioDefinition->TargetRoundCount);
	}

	TransitionToPhase(EIdiotsTogetherRoundPhase::Waiting);

	// Automatically transition from Waiting to Briefing upon lifecycle start
	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
	{
		TransitionToPhase(EIdiotsTogetherRoundPhase::Briefing);
	}));
}

void AIdiotsTogetherGameModeBase::TransitionToPhase(EIdiotsTogetherRoundPhase NewPhase)
{
	if (!HasAuthority())
	{
		return;
	}

	AIdiotsTogetherGameStateBase* GS = GetGameState<AIdiotsTogetherGameStateBase>();
	if (!GS)
	{
		return;
	}

	// Handle NextRound transition logic
	if (NewPhase == EIdiotsTogetherRoundPhase::NextRound)
	{
		GS->SetCurrentRoundPhase_Server(EIdiotsTogetherRoundPhase::NextRound);

		int32 TargetRounds = ActiveScenarioDefinition ? ActiveScenarioDefinition->TargetRoundCount : 3;
		GS->CurrentRoundIndex++;
		if (GS->CurrentRoundIndex > TargetRounds)
		{
			UE_LOG(LogIdiotsTogetherRound, Log, TEXT("Round limit reached (%d/%d); wrapping to round 1 (run-end flow arrives in P2.2)."),
				GS->CurrentRoundIndex, TargetRounds);
			GS->CurrentRoundIndex = 1;
		}

		UE_LOG(LogIdiotsTogetherRound, Log, TEXT("Advancing to round %d"), GS->CurrentRoundIndex);

		// Briefly show NextRound then transition to Briefing
		GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		GetWorldTimerManager().SetTimer(PhaseTimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			TransitionToPhase(EIdiotsTogetherRoundPhase::Briefing);
		}), 1.5f, false);
		return;
	}

	GS->SetCurrentRoundPhase_Server(NewPhase);

	GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

	CurrentPhaseDuration = 0.0f;

	switch (NewPhase)
	{
	case EIdiotsTogetherRoundPhase::Waiting:
		CurrentPhaseDuration = 0.0f;
		break;
	case EIdiotsTogetherRoundPhase::Briefing:
		CurrentPhaseDuration = ActiveScenarioDefinition ? ActiveScenarioDefinition->BriefingDuration : 5.0f;
		break;
	case EIdiotsTogetherRoundPhase::Planning:
		CurrentPhaseDuration = ActiveScenarioDefinition ? ActiveScenarioDefinition->PlanningDuration : 5.0f;
		break;
	case EIdiotsTogetherRoundPhase::Execution:
		CurrentPhaseDuration = ActiveScenarioDefinition ? ActiveScenarioDefinition->ExecutionDuration : 20.0f;
		// Initialize runtime objectives from scenario definition
		GS->ObjectiveStates.Empty();
		if (ActiveScenarioDefinition)
		{
			for (const FIdiotsTogetherObjectiveDefinition& ObjDef : ActiveScenarioDefinition->Objectives)
			{
				FIdiotsTogetherObjectiveState State;
				State.ObjectiveID = ObjDef.ObjectiveID;
				State.DisplayName = ObjDef.DisplayName;
				State.Description = ObjDef.Description;
				State.bIsRequired = ObjDef.bIsRequired;
				State.bIsCompleted = false;
				GS->ObjectiveStates.Add(State);
			}
		}
		GS->OnObjectiveStatesChanged.Broadcast();
		UE_LOG(LogIdiotsTogetherObjective, Log, TEXT("Execution started with %d objectives"), GS->ObjectiveStates.Num());
		break;
	case EIdiotsTogetherRoundPhase::Success:
	case EIdiotsTogetherRoundPhase::Failure:
		CurrentPhaseDuration = 3.0f;
		break;
	case EIdiotsTogetherRoundPhase::Results:
		CurrentPhaseDuration = ActiveScenarioDefinition ? ActiveScenarioDefinition->ResultsDuration : 5.0f;
		break;
	default:
		break;
	}

	PhaseTimeRemaining = CurrentPhaseDuration;
	GS->RemainingPhaseTime = PhaseTimeRemaining;

	UE_LOG(LogIdiotsTogetherRound, Log, TEXT("Phase -> %s (duration %.1fs, round %d)"),
		GetRoundPhaseString(NewPhase), CurrentPhaseDuration, GS->CurrentRoundIndex);

	if (CurrentPhaseDuration > 0.0f)
	{
		GetWorldTimerManager().SetTimer(PhaseTimerHandle, this, &AIdiotsTogetherGameModeBase::OnPhaseTimerExpired, CurrentPhaseDuration, false);
		GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AIdiotsTogetherGameModeBase::UpdatePhaseCountdown, 0.25f, true);
	}
}

void AIdiotsTogetherGameModeBase::UpdatePhaseCountdown()
{
	if (!HasAuthority())
	{
		return;
	}

	PhaseTimeRemaining = FMath::Max(0.0f, GetWorldTimerManager().GetTimerRemaining(PhaseTimerHandle));
	if (AIdiotsTogetherGameStateBase* GS = GetGameState<AIdiotsTogetherGameStateBase>())
	{
		GS->RemainingPhaseTime = PhaseTimeRemaining;
	}
}

void AIdiotsTogetherGameModeBase::OnPhaseTimerExpired()
{
	if (!HasAuthority())
	{
		return;
	}

	AIdiotsTogetherGameStateBase* GS = GetGameState<AIdiotsTogetherGameStateBase>();
	if (!GS)
	{
		return;
	}

	switch (GS->CurrentRoundPhase)
	{
	case EIdiotsTogetherRoundPhase::Waiting:
		TransitionToPhase(EIdiotsTogetherRoundPhase::Briefing);
		break;
	case EIdiotsTogetherRoundPhase::Briefing:
		TransitionToPhase(EIdiotsTogetherRoundPhase::Planning);
		break;
	case EIdiotsTogetherRoundPhase::Planning:
		TransitionToPhase(EIdiotsTogetherRoundPhase::Execution);
		break;
	case EIdiotsTogetherRoundPhase::Execution:
		// Timer expiration during Execution when required objectives are incomplete results in Failure
		if (CheckAllRequiredObjectivesCompleted())
		{
			UE_LOG(LogIdiotsTogetherRound, Log, TEXT("Execution timer expired with all required objectives complete -> Success"));
			TransitionToPhase(EIdiotsTogetherRoundPhase::Success);
		}
		else
		{
			UE_LOG(LogIdiotsTogetherRound, Log, TEXT("Execution timer expired with required objectives incomplete -> Failure"));
			TransitionToPhase(EIdiotsTogetherRoundPhase::Failure);
		}
		break;
	case EIdiotsTogetherRoundPhase::Success:
	case EIdiotsTogetherRoundPhase::Failure:
		TransitionToPhase(EIdiotsTogetherRoundPhase::Results);
		break;
	case EIdiotsTogetherRoundPhase::Results:
		TransitionToPhase(EIdiotsTogetherRoundPhase::NextRound);
		break;
	default:
		break;
	}
}

void AIdiotsTogetherGameModeBase::AdvancePhase()
{
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogIdiotsTogetherDev, Log, TEXT("[%s] AdvancePhase()"), *GetNetworkRoleString(this));
	OnPhaseTimerExpired();
}

void AIdiotsTogetherGameModeBase::ForcePhaseSuccess()
{
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogIdiotsTogetherDev, Log, TEXT("[%s] ForcePhaseSuccess()"), *GetNetworkRoleString(this));
	TransitionToPhase(EIdiotsTogetherRoundPhase::Success);
}

void AIdiotsTogetherGameModeBase::ForcePhaseFailure()
{
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogIdiotsTogetherDev, Log, TEXT("[%s] ForcePhaseFailure()"), *GetNetworkRoleString(this));
	TransitionToPhase(EIdiotsTogetherRoundPhase::Failure);
}

bool AIdiotsTogetherGameModeBase::CompleteObjective(const FString& ObjectiveID)
{
	if (!HasAuthority())
	{
		return false;
	}

	AIdiotsTogetherGameStateBase* GS = GetGameState<AIdiotsTogetherGameStateBase>();
	if (!GS || GS->CurrentRoundPhase != EIdiotsTogetherRoundPhase::Execution)
	{
		UE_LOG(LogIdiotsTogetherObjective, Verbose, TEXT("Ignored completion of '%s': not in Execution"), *ObjectiveID);
		return false;
	}

	bool bFoundAndUpdated = false;
	for (FIdiotsTogetherObjectiveState& State : GS->ObjectiveStates)
	{
		if (State.ObjectiveID.Equals(ObjectiveID, ESearchCase::IgnoreCase))
		{
			if (State.bIsCompleted)
			{
				UE_LOG(LogIdiotsTogetherObjective, Verbose, TEXT("Objective '%s' already completed"), *ObjectiveID);
				return false;
			}
			State.bIsCompleted = true;
			bFoundAndUpdated = true;
			UE_LOG(LogIdiotsTogetherObjective, Log, TEXT("Objective completed: %s ('%s')"),
				*State.ObjectiveID, *State.DisplayName.ToString());
			break;
		}
	}

	if (bFoundAndUpdated)
	{
		GS->OnObjectiveStatesChanged.Broadcast();

		if (CheckAllRequiredObjectivesCompleted())
		{
			UE_LOG(LogIdiotsTogetherRound, Log, TEXT("All required objectives complete -> early Success"));
			GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
			GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
			TransitionToPhase(EIdiotsTogetherRoundPhase::Success);
		}
		return true;
	}

	UE_LOG(LogIdiotsTogetherObjective, Warning, TEXT("Objective '%s' is not part of the active scenario"), *ObjectiveID);
	return false;
}

bool AIdiotsTogetherGameModeBase::CheckAllRequiredObjectivesCompleted() const
{
	const AIdiotsTogetherGameStateBase* GS = GetGameState<AIdiotsTogetherGameStateBase>();
	if (!GS)
	{
		return false;
	}

	int32 RequiredCount = 0;
	int32 RequiredCompletedCount = 0;

	for (const FIdiotsTogetherObjectiveState& State : GS->ObjectiveStates)
	{
		if (State.bIsRequired)
		{
			RequiredCount++;
			if (State.bIsCompleted)
			{
				RequiredCompletedCount++;
			}
		}
	}

	return RequiredCount == 0 || (RequiredCompletedCount >= RequiredCount);
}
