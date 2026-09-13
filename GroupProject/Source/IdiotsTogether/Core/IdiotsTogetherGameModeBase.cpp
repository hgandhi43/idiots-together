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
		CurrentPhaseDuration = ActiveScenarioDefinition ? ActiveScenarioDefinition->ExecutionDuration : 10.0f;
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
		// Default timer expiration during Execution in M3B transitions to Success
		TransitionToPhase(EIdiotsTogetherRoundPhase::Success);
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
