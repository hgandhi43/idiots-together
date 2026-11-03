#include "IdiotsTogetherTestCommon.h"
#include "IdiotsTogether/Core/IdiotsTogetherPhaseRules.h"
#include "IdiotsTogether/Core/IdiotsTogetherObjectiveRules.h"
#include "IdiotsTogether/Core/IdiotsTogetherGameplayTags.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{

	bool PhaseRulesTest_PhaseIs(EIdiotsTogetherRoundPhase Actual, EIdiotsTogetherRoundPhase Expected)
	{
		return Actual == Expected;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIdiotsTogetherPhaseTransitionOrderTest, "IdiotsTogether.Rules.Phase.TransitionOrder", IDIOTSTOGETHER_TEST_FLAGS)
bool FIdiotsTogetherPhaseTransitionOrderTest::RunTest(const FString& Parameters)
{
	using namespace IdiotsTogetherPhaseRules;
	using EPhase = EIdiotsTogetherRoundPhase;

	// Successful round walked purely by timers.
	const TArray<EPhase> Expected = { EPhase::Waiting, EPhase::Briefing, EPhase::Planning, EPhase::Execution, EPhase::Success, EPhase::Results, EPhase::NextRound, EPhase::Briefing };
	EPhase Phase = Expected[0];
	for (int32 Step = 1; Step < Expected.Num(); ++Step)
	{
		Phase = GetNextPhaseOnTimeout(Phase, /*bAllRequiredObjectivesComplete*/ true);
		TestTrue(FString::Printf(TEXT("Step %d is %s"), Step, GetRoundPhaseString(Expected[Step])), PhaseRulesTest_PhaseIs(Phase, Expected[Step]));
	}

	// Failure branch also leads to Results.
	TestTrue(TEXT("Failure -> Results"), PhaseRulesTest_PhaseIs(GetNextPhaseOnTimeout(EPhase::Failure, false), EPhase::Results));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIdiotsTogetherPhaseTimeoutFailureTest, "IdiotsTogether.Rules.Phase.TimeoutFailure", IDIOTSTOGETHER_TEST_FLAGS)
bool FIdiotsTogetherPhaseTimeoutFailureTest::RunTest(const FString& Parameters)
{
	using EPhase = EIdiotsTogetherRoundPhase;

	TArray<FIdiotsTogetherObjectiveDefinition> Defs;
	FIdiotsTogetherObjectiveDefinition& Required = Defs.AddDefaulted_GetRef();
	Required.ObjectiveTag = IdiotsTogetherTags::Objective_Test_Primary;
	Required.bIsRequired = true;

	TArray<FIdiotsTogetherObjectiveState> States = IdiotsTogetherObjectiveRules::BuildInitialStates(Defs);
	const bool bAllDone = IdiotsTogetherObjectiveRules::AreAllRequiredComplete(States);

	TestFalse(TEXT("Required objective starts incomplete"), bAllDone);
	TestTrue(TEXT("Execution timeout with incomplete required objective -> Failure"),
		PhaseRulesTest_PhaseIs(IdiotsTogetherPhaseRules::GetNextPhaseOnTimeout(EPhase::Execution, bAllDone), EPhase::Failure));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIdiotsTogetherPhaseEarlySuccessTest, "IdiotsTogether.Rules.Phase.EarlySuccess", IDIOTSTOGETHER_TEST_FLAGS)
bool FIdiotsTogetherPhaseEarlySuccessTest::RunTest(const FString& Parameters)
{
	using EPhase = EIdiotsTogetherRoundPhase;

	TArray<FIdiotsTogetherObjectiveDefinition> Defs;
	FIdiotsTogetherObjectiveDefinition& Required = Defs.AddDefaulted_GetRef();
	Required.ObjectiveTag = IdiotsTogetherTags::Objective_Test_Primary;
	Required.bIsRequired = true;
	FIdiotsTogetherObjectiveDefinition& Optional = Defs.AddDefaulted_GetRef();
	Optional.ObjectiveTag = IdiotsTogetherTags::Objective_Test_Optional;
	Optional.bIsRequired = false;

	TArray<FIdiotsTogetherObjectiveState> States = IdiotsTogetherObjectiveRules::BuildInitialStates(Defs);
	IdiotsTogetherObjectiveRules::TryComplete(States, IdiotsTogetherTags::Objective_Test_Primary);

	// The GameMode ends Execution early as soon as this becomes true; optional objectives do not block it.
	TestTrue(TEXT("Completing the only required objective satisfies the round"), IdiotsTogetherObjectiveRules::AreAllRequiredComplete(States));
	TestTrue(TEXT("Execution timeout with required objectives done -> Success"),
		PhaseRulesTest_PhaseIs(IdiotsTogetherPhaseRules::GetNextPhaseOnTimeout(EPhase::Execution, true), EPhase::Success));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIdiotsTogetherPhaseDurationsTest, "IdiotsTogether.Rules.Phase.Durations", IDIOTSTOGETHER_TEST_FLAGS)
bool FIdiotsTogetherPhaseDurationsTest::RunTest(const FString& Parameters)
{
	using namespace IdiotsTogetherPhaseRules;
	using EPhase = EIdiotsTogetherRoundPhase;

	FIdiotsTogetherPhaseDurations D;
	D.Briefing = 1.0f; D.Planning = 2.0f; D.Execution = 3.0f; D.Outcome = 4.0f; D.Results = 5.0f; D.NextRound = 6.0f;

	TestEqual(TEXT("Waiting is untimed"), GetPhaseDuration(EPhase::Waiting, D), 0.0f);
	TestEqual(TEXT("Briefing"), GetPhaseDuration(EPhase::Briefing, D), 1.0f);
	TestEqual(TEXT("Planning"), GetPhaseDuration(EPhase::Planning, D), 2.0f);
	TestEqual(TEXT("Execution"), GetPhaseDuration(EPhase::Execution, D), 3.0f);
	TestEqual(TEXT("Success uses Outcome"), GetPhaseDuration(EPhase::Success, D), 4.0f);
	TestEqual(TEXT("Failure uses Outcome"), GetPhaseDuration(EPhase::Failure, D), 4.0f);
	TestEqual(TEXT("Results"), GetPhaseDuration(EPhase::Results, D), 5.0f);
	TestEqual(TEXT("NextRound"), GetPhaseDuration(EPhase::NextRound, D), 6.0f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIdiotsTogetherPhaseRoundAdvanceTest, "IdiotsTogether.Rules.Phase.RoundAdvance", IDIOTSTOGETHER_TEST_FLAGS)
bool FIdiotsTogetherPhaseRoundAdvanceTest::RunTest(const FString& Parameters)
{
	using namespace IdiotsTogetherPhaseRules;

	bool bRunComplete = true;
	TestEqual(TEXT("Round 1 of 3 -> 2"), GetNextRoundIndex(1, 3, bRunComplete), 2);
	TestFalse(TEXT("Run not complete after round 1"), bRunComplete);

	TestEqual(TEXT("Round 3 of 3 wraps to 1"), GetNextRoundIndex(3, 3, bRunComplete), 1);
	TestTrue(TEXT("Run complete after last round"), bRunComplete);

	TestEqual(TEXT("Target 0 is treated as 1"), GetNextRoundIndex(1, 0, bRunComplete), 1);
	TestTrue(TEXT("Single-round run completes"), bRunComplete);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
