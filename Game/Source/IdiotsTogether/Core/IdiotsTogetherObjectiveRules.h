#pragma once

#include "CoreMinimal.h"
#include "IdiotsTogetherTypes.h"

/** Outcome of trying to complete an objective. */
enum class EIdiotsTogetherObjectiveCompleteResult : uint8
{
	Completed,
	AlreadyCompleted,
	NotFound
};

/**
 * Pure (world-free) objective bookkeeping used by the GameMode. Unit-tested in Tests/.
 */
namespace IdiotsTogetherObjectiveRules
{
	/** Fresh runtime states (all incomplete) for a scenario's objective definitions. */
	IDIOTSTOGETHER_API TArray<FIdiotsTogetherObjectiveState> BuildInitialStates(const TArray<FIdiotsTogetherObjectiveDefinition>& Definitions);

	/** Marks the objective with exactly this tag complete. Tag matching is exact (no parent matching). */
	IDIOTSTOGETHER_API EIdiotsTogetherObjectiveCompleteResult TryComplete(TArray<FIdiotsTogetherObjectiveState>& States, const FGameplayTag& ObjectiveTag);

	/** True when every required objective is complete (also true when there are none). */
	IDIOTSTOGETHER_API bool AreAllRequiredComplete(const TArray<FIdiotsTogetherObjectiveState>& States);

	/** The next objective a dev "complete objective" command should finish: first incomplete required, else first incomplete optional. Invalid tag if none. */
	IDIOTSTOGETHER_API FGameplayTag FindNextIncomplete(const TArray<FIdiotsTogetherObjectiveState>& States);
}
