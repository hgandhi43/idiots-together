#pragma once

#include "CoreMinimal.h"
#include "IdiotsTogetherTypes.h"

/** Seconds each timed phase lasts. Built by the GameMode from the scenario definition + its own tunables. */
struct FIdiotsTogetherPhaseDurations
{
	float Briefing = 0.0f;
	float Planning = 0.0f;
	float Execution = 0.0f;
	/** Success / Failure banner. */
	float Outcome = 0.0f;
	float Results = 0.0f;
	/** Short NextRound interstitial before the next Briefing. */
	float NextRound = 0.0f;
};

/**
 * Pure (world-free) round state machine rules. The GameMode owns timers and replication;
 * these functions decide *what* happens. Unit-tested in Tests/.
 *
 * Order: Waiting -> Briefing -> Planning -> Execution -> (Success | Failure) -> Results -> NextRound -> Briefing ...
 * Execution also ends early with Success as soon as all required objectives are complete (GameMode::CompleteObjective).
 */
namespace IdiotsTogetherPhaseRules
{
	/** Phase entered when Current's timer runs out (or a dev "advance phase" is issued). */
	IDIOTSTOGETHER_API EIdiotsTogetherRoundPhase GetNextPhaseOnTimeout(EIdiotsTogetherRoundPhase Current, bool bAllRequiredObjectivesComplete);

	/** Duration of Phase; 0 means "untimed" (Waiting). */
	IDIOTSTOGETHER_API float GetPhaseDuration(EIdiotsTogetherRoundPhase Phase, const FIdiotsTogetherPhaseDurations& Durations);

	/**
	 * Round index after a NextRound. Sets bOutRunComplete when CurrentIndex was the last round.
	 * Until the run-end flow exists (P2.2) the index then wraps back to 1.
	 */
	IDIOTSTOGETHER_API int32 GetNextRoundIndex(int32 CurrentIndex, int32 TargetRoundCount, bool& bOutRunComplete);
}
