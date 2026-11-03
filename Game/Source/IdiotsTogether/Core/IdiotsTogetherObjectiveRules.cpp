#include "IdiotsTogetherObjectiveRules.h"

namespace IdiotsTogetherObjectiveRules
{
	TArray<FIdiotsTogetherObjectiveState> BuildInitialStates(const TArray<FIdiotsTogetherObjectiveDefinition>& Definitions)
	{
		TArray<FIdiotsTogetherObjectiveState> States;
		States.Reserve(Definitions.Num());

		for (const FIdiotsTogetherObjectiveDefinition& Definition : Definitions)
		{
			FIdiotsTogetherObjectiveState& State = States.AddDefaulted_GetRef();
			State.ObjectiveTag = Definition.ObjectiveTag;
			State.DisplayName = Definition.DisplayName;
			State.Description = Definition.Description;
			State.bIsRequired = Definition.bIsRequired;
			State.bIsCompleted = false;
		}

		return States;
	}

	EIdiotsTogetherObjectiveCompleteResult TryComplete(TArray<FIdiotsTogetherObjectiveState>& States, const FGameplayTag& ObjectiveTag)
	{
		if (!ObjectiveTag.IsValid())
		{
			return EIdiotsTogetherObjectiveCompleteResult::NotFound;
		}

		for (FIdiotsTogetherObjectiveState& State : States)
		{
			if (State.ObjectiveTag == ObjectiveTag)
			{
				if (State.bIsCompleted)
				{
					return EIdiotsTogetherObjectiveCompleteResult::AlreadyCompleted;
				}

				State.bIsCompleted = true;
				return EIdiotsTogetherObjectiveCompleteResult::Completed;
			}
		}

		return EIdiotsTogetherObjectiveCompleteResult::NotFound;
	}

	bool AreAllRequiredComplete(const TArray<FIdiotsTogetherObjectiveState>& States)
	{
		for (const FIdiotsTogetherObjectiveState& State : States)
		{
			if (State.bIsRequired && !State.bIsCompleted)
			{
				return false;
			}
		}
		return true;
	}

	FGameplayTag FindNextIncomplete(const TArray<FIdiotsTogetherObjectiveState>& States)
	{
		for (const bool bWantRequired : { true, false })
		{
			for (const FIdiotsTogetherObjectiveState& State : States)
			{
				if (State.bIsRequired == bWantRequired && !State.bIsCompleted)
				{
					return State.ObjectiveTag;
				}
			}
		}
		return FGameplayTag();
	}
}
