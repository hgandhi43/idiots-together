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
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] GameMode BeginPlay: Class=%s, DefaultPawnClass=%s, PlayerControllerClass=%s"),
		*GetClass()->GetName(),
		DefaultPawnClass ? *DefaultPawnClass->GetName() : TEXT("NULL"),
		PlayerControllerClass ? *PlayerControllerClass->GetName() : TEXT("NULL"));

	if (!ActiveScenarioDefinition)
	{
		ActiveScenarioDefinition = NewObject<UIdiotsTogetherScenarioDefinition>(this, UIdiotsTogetherScenarioDefinition::StaticClass());
		UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B GAMEMODE] Created default UIdiotsTogetherScenarioDefinition instance (TestScenario)."));
	}

	StartScenarioLifecycle();
}

void AIdiotsTogetherGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogIdiotsTogether, Log, TEXT("Player Joined: %s"), *NewPlayer->GetName());
}

void AIdiotsTogetherGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (Exiting)
	{
		UE_LOG(LogIdiotsTogether, Log, TEXT("Player Left: %s"), *Exiting->GetName());
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
		UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3C GAMEMODE] Started Scenario Lifecycle for ScenarioID=%s ('%s'), TargetRounds=%d"),
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
			UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B GAMEMODE] Scenario complete! Reached round limit (%d/%d). Resetting to Round 1."),
				GS->CurrentRoundIndex, TargetRounds);
			GS->CurrentRoundIndex = 1;
		}

		UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B GAMEMODE] Advancing to Round %d"), GS->CurrentRoundIndex);

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
		UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3D GAMEMODE] Execution phase started with %d active objectives."), GS->ObjectiveStates.Num());
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

	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B GAMEMODE] Transitioned to Phase=%d (%s), Duration=%.1fs, Round=%d"),
		static_cast<int32>(NewPhase), *UEnum::GetValueAsString(NewPhase), CurrentPhaseDuration, GS->CurrentRoundIndex);

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
			UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3D GAMEMODE] Execution timer expired and all required objectives were complete -> SUCCESS"));
			TransitionToPhase(EIdiotsTogetherRoundPhase::Success);
		}
		else
		{
			UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3D GAMEMODE] Execution timer expired with incomplete required objectives -> FAILURE"));
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

	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B][%s] GameMode AdvancePhase()"), *GetNetworkRoleString(this));
	OnPhaseTimerExpired();
}

void AIdiotsTogetherGameModeBase::ForcePhaseSuccess()
{
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B][%s] GameMode ForcePhaseSuccess()"), *GetNetworkRoleString(this));
	TransitionToPhase(EIdiotsTogetherRoundPhase::Success);
}

void AIdiotsTogetherGameModeBase::ForcePhaseFailure()
{
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B][%s] GameMode ForcePhaseFailure()"), *GetNetworkRoleString(this));
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
		UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3D GAMEMODE] Cannot complete objective '%s' - Phase is not Execution!"), *ObjectiveID);
		return false;
	}

	bool bFoundAndUpdated = false;
	for (FIdiotsTogetherObjectiveState& State : GS->ObjectiveStates)
	{
		if (State.ObjectiveID.Equals(ObjectiveID, ESearchCase::IgnoreCase))
		{
			if (State.bIsCompleted)
			{
				UE_LOG(LogIdiotsTogether, Log, TEXT("[M3D GAMEMODE] Objective '%s' is already completed."), *ObjectiveID);
				return false;
			}
			State.bIsCompleted = true;
			bFoundAndUpdated = true;
			UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3D GAMEMODE] Objective '%s' ('%s') COMPLETED!"),
				*State.ObjectiveID, *State.DisplayName.ToString());
			break;
		}
	}

	if (bFoundAndUpdated)
	{
		GS->OnObjectiveStatesChanged.Broadcast();

		if (CheckAllRequiredObjectivesCompleted())
		{
			UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3D GAMEMODE] All required objectives COMPLETED! Transitioning early to SUCCESS."));
			GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
			GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
			TransitionToPhase(EIdiotsTogetherRoundPhase::Success);
		}
		return true;
	}

	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3D GAMEMODE] ObjectiveID '%s' not found in active objective states!"), *ObjectiveID);
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
