#include "IdiotsTogetherGameModeBase.h"
#include "IdiotsTogetherPlayerController.h"
#include "IdiotsTogetherGameStateBase.h"
#include "IdiotsTogetherPlayerState.h"
#include "IdiotsTogetherCharacter.h"
#include "IdiotsTogetherScenarioDefinition.h"
#include "IdiotsTogetherCharacterRules.h"
#include "IdiotsTogetherObjectiveRules.h"
#include "IdiotsTogetherPhaseRules.h"
#include "IdiotsTogetherSettings.h"
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
	// Assign before Super so the pawn spawned by HandleStartingNewPlayer already has its colour.
	AssignPlayerColor(NewPlayer);
	Super::PostLogin(NewPlayer);
	UE_LOG(LogIdiotsTogetherPlayer, Log, TEXT("Player joined: %s"), *GetNameSafe(NewPlayer));
}

void AIdiotsTogetherGameModeBase::AssignPlayerColor(APlayerController* NewPlayer)
{
	AIdiotsTogetherPlayerState* NewPS = NewPlayer ? NewPlayer->GetPlayerState<AIdiotsTogetherPlayerState>() : nullptr;
	if (!NewPS || !GameState)
	{
		return;
	}

	TArray<int32> TakenColors;
	for (const APlayerState* OtherBase : GameState->PlayerArray)
	{
		const AIdiotsTogetherPlayerState* Other = Cast<AIdiotsTogetherPlayerState>(OtherBase);
		if (Other && Other != NewPS && Other->GetPlayerColorIndex() != INDEX_NONE)
		{
			TakenColors.Add(Other->GetPlayerColorIndex());
		}
	}

	const int32 PaletteSize = UIdiotsTogetherSettings::Get()->PlayerColorPalette.Num();
	NewPS->SetPlayerColorIndex_Server(IdiotsTogetherCharacterRules::PickColorIndex(TakenColors, PaletteSize));
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
		GS->ActiveScenarioTag = ActiveScenarioDefinition->ScenarioTag;
		GS->ScenarioDisplayName = ActiveScenarioDefinition->DisplayName;
		GS->ScenarioDescription = ActiveScenarioDefinition->Description;
		GS->CurrentRoundIndex = 1;
		UE_LOG(LogIdiotsTogetherRound, Log, TEXT("Scenario lifecycle started: %s ('%s'), %d rounds"),
			*GS->ActiveScenarioTag.ToString(), *GS->ScenarioDisplayName.ToString(), ActiveScenarioDefinition->TargetRoundCount);
	}

	TransitionToPhase(EIdiotsTogetherRoundPhase::Waiting);

	// Automatically transition from Waiting to Briefing upon lifecycle start
	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
	{
		TransitionToPhase(EIdiotsTogetherRoundPhase::Briefing);
	}));
}

FIdiotsTogetherPhaseDurations AIdiotsTogetherGameModeBase::GetPhaseDurations() const
{
	FIdiotsTogetherPhaseDurations Durations;
	if (ActiveScenarioDefinition)
	{
		Durations.Briefing = ActiveScenarioDefinition->BriefingDuration;
		Durations.Planning = ActiveScenarioDefinition->PlanningDuration;
		Durations.Execution = ActiveScenarioDefinition->ExecutionDuration;
		Durations.Results = ActiveScenarioDefinition->ResultsDuration;
	}
	Durations.Outcome = OutcomeDisplayDuration;
	Durations.NextRound = NextRoundDisplayDuration;
	return Durations;
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

	GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);

	GS->SetCurrentRoundPhase_Server(NewPhase);

	switch (NewPhase)
	{
	case EIdiotsTogetherRoundPhase::Execution:
		GS->ObjectiveStates = ActiveScenarioDefinition
			? IdiotsTogetherObjectiveRules::BuildInitialStates(ActiveScenarioDefinition->Objectives)
			: TArray<FIdiotsTogetherObjectiveState>();
		GS->OnObjectiveStatesChanged.Broadcast();
		UE_LOG(LogIdiotsTogetherObjective, Log, TEXT("Execution started with %d objectives"), GS->ObjectiveStates.Num());
		break;

	case EIdiotsTogetherRoundPhase::NextRound:
	{
		const int32 TargetRounds = ActiveScenarioDefinition ? ActiveScenarioDefinition->TargetRoundCount : 1;
		bool bRunComplete = false;
		GS->CurrentRoundIndex = IdiotsTogetherPhaseRules::GetNextRoundIndex(GS->CurrentRoundIndex, TargetRounds, bRunComplete);
		if (bRunComplete)
		{
			UE_LOG(LogIdiotsTogetherRound, Log, TEXT("All %d rounds played; wrapping to round 1 (run-end flow arrives in P2.2)."), TargetRounds);
		}
		break;
	}

	default:
		break;
	}

	CurrentPhaseDuration = IdiotsTogetherPhaseRules::GetPhaseDuration(NewPhase, GetPhaseDurations());
	PhaseTimeRemaining = CurrentPhaseDuration;
	GS->RemainingPhaseTime = PhaseTimeRemaining;

	UE_LOG(LogIdiotsTogetherRound, Log, TEXT("Phase -> %s (duration %.1fs, round %d)"),
		GetRoundPhaseString(NewPhase), CurrentPhaseDuration, GS->CurrentRoundIndex);

	if (CurrentPhaseDuration > 0.0f)
	{
		GetWorldTimerManager().SetTimer(PhaseTimerHandle, this, &AIdiotsTogetherGameModeBase::OnPhaseTimerExpired, CurrentPhaseDuration, false);
		GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &AIdiotsTogetherGameModeBase::UpdatePhaseCountdown, CountdownReplicationInterval, true);
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

	const AIdiotsTogetherGameStateBase* GS = GetGameState<AIdiotsTogetherGameStateBase>();
	if (!GS)
	{
		return;
	}

	const EIdiotsTogetherRoundPhase Current = GS->CurrentRoundPhase;
	const EIdiotsTogetherRoundPhase Next = IdiotsTogetherPhaseRules::GetNextPhaseOnTimeout(Current, CheckAllRequiredObjectivesCompleted());

	if (Current == EIdiotsTogetherRoundPhase::Execution)
	{
		UE_LOG(LogIdiotsTogetherRound, Log, TEXT("Execution timer expired -> %s"), GetRoundPhaseString(Next));
	}

	if (Next != Current)
	{
		TransitionToPhase(Next);
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

bool AIdiotsTogetherGameModeBase::CompleteObjective(FGameplayTag ObjectiveTag)
{
	if (!HasAuthority())
	{
		return false;
	}

	AIdiotsTogetherGameStateBase* GS = GetGameState<AIdiotsTogetherGameStateBase>();
	if (!GS || GS->CurrentRoundPhase != EIdiotsTogetherRoundPhase::Execution)
	{
		UE_LOG(LogIdiotsTogetherObjective, Verbose, TEXT("Ignored completion of '%s': not in Execution"), *ObjectiveTag.ToString());
		return false;
	}

	switch (IdiotsTogetherObjectiveRules::TryComplete(GS->ObjectiveStates, ObjectiveTag))
	{
	case EIdiotsTogetherObjectiveCompleteResult::AlreadyCompleted:
		UE_LOG(LogIdiotsTogetherObjective, Verbose, TEXT("Objective '%s' already completed"), *ObjectiveTag.ToString());
		return false;

	case EIdiotsTogetherObjectiveCompleteResult::NotFound:
		UE_LOG(LogIdiotsTogetherObjective, Warning, TEXT("Objective '%s' is not part of the active scenario"), *ObjectiveTag.ToString());
		return false;

	case EIdiotsTogetherObjectiveCompleteResult::Completed:
	default:
		break;
	}

	UE_LOG(LogIdiotsTogetherObjective, Log, TEXT("Objective completed: %s"), *ObjectiveTag.ToString());
	GS->OnObjectiveStatesChanged.Broadcast(); // Server-side listeners; clients get OnRep_ObjectiveStates.

	if (CheckAllRequiredObjectivesCompleted())
	{
		UE_LOG(LogIdiotsTogetherRound, Log, TEXT("All required objectives complete -> early Success"));
		TransitionToPhase(EIdiotsTogetherRoundPhase::Success);
	}
	return true;
}

bool AIdiotsTogetherGameModeBase::CheckAllRequiredObjectivesCompleted() const
{
	const AIdiotsTogetherGameStateBase* GS = GetGameState<AIdiotsTogetherGameStateBase>();
	return GS && IdiotsTogetherObjectiveRules::AreAllRequiredComplete(GS->ObjectiveStates);
}

bool AIdiotsTogetherGameModeBase::DevCompleteNextObjective()
{
#if !UE_BUILD_SHIPPING
	const AIdiotsTogetherGameStateBase* GS = GetGameState<AIdiotsTogetherGameStateBase>();
	if (!GS)
	{
		return false;
	}

	const FGameplayTag NextTag = IdiotsTogetherObjectiveRules::FindNextIncomplete(GS->ObjectiveStates);
	return NextTag.IsValid() && CompleteObjective(NextTag);
#else
	return false;
#endif
}
