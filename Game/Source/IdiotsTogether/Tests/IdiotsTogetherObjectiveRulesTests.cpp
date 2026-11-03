#include "IdiotsTogetherTestCommon.h"
#include "IdiotsTogether/Core/IdiotsTogetherObjectiveRules.h"
#include "IdiotsTogether/Core/IdiotsTogetherGameplayTags.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{

	TArray<FIdiotsTogetherObjectiveDefinition> ObjectiveRulesTest_MakeDefinitions()
	{
		TArray<FIdiotsTogetherObjectiveDefinition> Defs;

		FIdiotsTogetherObjectiveDefinition& Required = Defs.AddDefaulted_GetRef();
		Required.ObjectiveTag = IdiotsTogetherTags::Objective_Test_Primary;
		Required.bIsRequired = true;

		FIdiotsTogetherObjectiveDefinition& Optional = Defs.AddDefaulted_GetRef();
		Optional.ObjectiveTag = IdiotsTogetherTags::Objective_Test_Optional;
		Optional.bIsRequired = false;

		return Defs;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIdiotsTogetherObjectiveCompleteTest, "IdiotsTogether.Rules.Objective.Complete", IDIOTSTOGETHER_TEST_FLAGS)
bool FIdiotsTogetherObjectiveCompleteTest::RunTest(const FString& Parameters)
{
	using namespace IdiotsTogetherObjectiveRules;

	TArray<FIdiotsTogetherObjectiveState> States = BuildInitialStates(ObjectiveRulesTest_MakeDefinitions());
	TestEqual(TEXT("One state per definition"), States.Num(), 2);
	TestFalse(TEXT("States start incomplete"), States[0].bIsCompleted || States[1].bIsCompleted);

	TestTrue(TEXT("First completion succeeds"),
		TryComplete(States, IdiotsTogetherTags::Objective_Test_Primary) == EIdiotsTogetherObjectiveCompleteResult::Completed);
	TestTrue(TEXT("Second completion reports AlreadyCompleted"),
		TryComplete(States, IdiotsTogetherTags::Objective_Test_Primary) == EIdiotsTogetherObjectiveCompleteResult::AlreadyCompleted);
	TestTrue(TEXT("Unknown tag reports NotFound"),
		TryComplete(States, IdiotsTogetherTags::Scenario_Test_Foundation) == EIdiotsTogetherObjectiveCompleteResult::NotFound);
	TestTrue(TEXT("Empty tag reports NotFound"),
		TryComplete(States, FGameplayTag()) == EIdiotsTogetherObjectiveCompleteResult::NotFound);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIdiotsTogetherObjectiveRequiredTest, "IdiotsTogether.Rules.Objective.RequiredOnly", IDIOTSTOGETHER_TEST_FLAGS)
bool FIdiotsTogetherObjectiveRequiredTest::RunTest(const FString& Parameters)
{
	using namespace IdiotsTogetherObjectiveRules;

	TArray<FIdiotsTogetherObjectiveState> States = BuildInitialStates(ObjectiveRulesTest_MakeDefinitions());
	TestFalse(TEXT("Not complete while required is open"), AreAllRequiredComplete(States));

	TryComplete(States, IdiotsTogetherTags::Objective_Test_Optional);
	TestFalse(TEXT("Optional alone does not complete the round"), AreAllRequiredComplete(States));

	TryComplete(States, IdiotsTogetherTags::Objective_Test_Primary);
	TestTrue(TEXT("Complete once required is done"), AreAllRequiredComplete(States));

	TestTrue(TEXT("No objectives counts as complete"), AreAllRequiredComplete(TArray<FIdiotsTogetherObjectiveState>()));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIdiotsTogetherObjectiveNextIncompleteTest, "IdiotsTogether.Rules.Objective.NextIncomplete", IDIOTSTOGETHER_TEST_FLAGS)
bool FIdiotsTogetherObjectiveNextIncompleteTest::RunTest(const FString& Parameters)
{
	using namespace IdiotsTogetherObjectiveRules;

	TArray<FIdiotsTogetherObjectiveDefinition> Defs = ObjectiveRulesTest_MakeDefinitions();
	Defs.Swap(0, 1); // Optional first in the list: required must still be picked first.
	TArray<FIdiotsTogetherObjectiveState> States = BuildInitialStates(Defs);

	TestTrue(TEXT("Required objective comes first"), FindNextIncomplete(States) == FGameplayTag(IdiotsTogetherTags::Objective_Test_Primary));
	TryComplete(States, IdiotsTogetherTags::Objective_Test_Primary);
	TestTrue(TEXT("Then the optional one"), FindNextIncomplete(States) == FGameplayTag(IdiotsTogetherTags::Objective_Test_Optional));
	TryComplete(States, IdiotsTogetherTags::Objective_Test_Optional);
	TestFalse(TEXT("Nothing left"), FindNextIncomplete(States).IsValid());
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
