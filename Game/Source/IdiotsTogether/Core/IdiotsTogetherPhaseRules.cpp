#include "IdiotsTogetherPhaseRules.h"

namespace IdiotsTogetherPhaseRules
{
	EIdiotsTogetherRoundPhase GetNextPhaseOnTimeout(EIdiotsTogetherRoundPhase Current, bool bAllRequiredObjectivesComplete)
	{
		switch (Current)
		{
		case EIdiotsTogetherRoundPhase::Waiting:   return EIdiotsTogetherRoundPhase::Briefing;
		case EIdiotsTogetherRoundPhase::Briefing:  return EIdiotsTogetherRoundPhase::Planning;
		case EIdiotsTogetherRoundPhase::Planning:  return EIdiotsTogetherRoundPhase::Execution;
		case EIdiotsTogetherRoundPhase::Execution:
			return bAllRequiredObjectivesComplete ? EIdiotsTogetherRoundPhase::Success : EIdiotsTogetherRoundPhase::Failure;
		case EIdiotsTogetherRoundPhase::Success:
		case EIdiotsTogetherRoundPhase::Failure:   return EIdiotsTogetherRoundPhase::Results;
		case EIdiotsTogetherRoundPhase::Results:   return EIdiotsTogetherRoundPhase::NextRound;
		case EIdiotsTogetherRoundPhase::NextRound: return EIdiotsTogetherRoundPhase::Briefing;
		default:                                   return Current;
		}
	}

	float GetPhaseDuration(EIdiotsTogetherRoundPhase Phase, const FIdiotsTogetherPhaseDurations& Durations)
	{
		switch (Phase)
		{
		case EIdiotsTogetherRoundPhase::Briefing:  return Durations.Briefing;
		case EIdiotsTogetherRoundPhase::Planning:  return Durations.Planning;
		case EIdiotsTogetherRoundPhase::Execution: return Durations.Execution;
		case EIdiotsTogetherRoundPhase::Success:
		case EIdiotsTogetherRoundPhase::Failure:   return Durations.Outcome;
		case EIdiotsTogetherRoundPhase::Results:   return Durations.Results;
		case EIdiotsTogetherRoundPhase::NextRound: return Durations.NextRound;
		case EIdiotsTogetherRoundPhase::Waiting:
		default:                                   return 0.0f;
		}
	}

	int32 GetNextRoundIndex(int32 CurrentIndex, int32 TargetRoundCount, bool& bOutRunComplete)
	{
		const int32 SafeTarget = FMath::Max(1, TargetRoundCount);
		const int32 Next = CurrentIndex + 1;
		bOutRunComplete = Next > SafeTarget;
		return bOutRunComplete ? 1 : Next;
	}
}
