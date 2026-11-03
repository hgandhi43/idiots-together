#include "IdiotsTogetherTestCommon.h"
#include "IdiotsTogether/Interaction/IdiotsTogetherInteractionRules.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIdiotsTogetherInteractionReachTest, "IdiotsTogether.Rules.Interaction.Reach", IDIOTSTOGETHER_TEST_FLAGS)
bool FIdiotsTogetherInteractionReachTest::RunTest(const FString& Parameters)
{
	using namespace IdiotsTogetherInteractionRules;

	// 100-unit cube centred 500 units in front of the viewer: near face at X=450.
	const FBox Cube(FVector(450.0f, -50.0f, -50.0f), FVector(550.0f, 50.0f, 50.0f));
	const FVector Eye = FVector::ZeroVector;

	TestTrue(TEXT("Reach measured to the near face, not the centre"), IsWithinReach(Eye, Cube, 460.0f));
	TestFalse(TEXT("Just short of the near face"), IsWithinReach(Eye, Cube, 440.0f));
	TestTrue(TEXT("Viewer inside the bounds is always in reach"), IsWithinReach(FVector(500.0f, 0.0f, 0.0f), Cube, 0.0f));
	TestFalse(TEXT("Invalid bounds are unreachable"), IsWithinReach(Eye, FBox(ForceInit), 10000.0f));
	TestFalse(TEXT("Negative reach is unreachable"), IsWithinReach(FVector(500.0f, 0.0f, 0.0f), Cube, -1.0f));
	TestEqual(TEXT("Closest point is on the near face"), GetClosestPoint(Eye, Cube), FVector(450.0f, 0.0f, 0.0f));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIdiotsTogetherInteractionRateLimitTest, "IdiotsTogether.Rules.Interaction.RateLimit", IDIOTSTOGETHER_TEST_FLAGS)
bool FIdiotsTogetherInteractionRateLimitTest::RunTest(const FString& Parameters)
{
	constexpr double Interval = 0.15;
	FIdiotsTogetherRateLimiter Limiter;

	TestTrue(TEXT("First request accepted"), Limiter.TryAccept(10.0, Interval));
	TestFalse(TEXT("Burst request rejected"), Limiter.TryAccept(10.05, Interval));
	TestTrue(TEXT("Request after the interval accepted"), Limiter.TryAccept(10.2, Interval));
	TestFalse(TEXT("Rejected requests do not extend the window"), Limiter.TryAccept(10.3, Interval));
	TestTrue(TEXT("Measured from last accepted, not last attempt"), Limiter.TryAccept(10.36, Interval));

	Limiter.Reset();
	TestTrue(TEXT("Reset accepts immediately"), Limiter.TryAccept(10.37, Interval));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
