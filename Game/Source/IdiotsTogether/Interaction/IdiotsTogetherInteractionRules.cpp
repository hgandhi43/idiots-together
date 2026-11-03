#include "IdiotsTogetherInteractionRules.h"

namespace IdiotsTogetherInteractionRules
{
	FVector GetClosestPoint(const FVector& ViewLocation, const FBox& TargetBounds)
	{
		return TargetBounds.GetClosestPointTo(ViewLocation);
	}

	bool IsWithinReach(const FVector& ViewLocation, const FBox& TargetBounds, float MaxDistance)
	{
		if (!TargetBounds.IsValid || MaxDistance < 0.0f)
		{
			return false;
		}

		const FVector Closest = GetClosestPoint(ViewLocation, TargetBounds);
		return FVector::DistSquared(ViewLocation, Closest) <= FMath::Square(MaxDistance);
	}
}

bool FIdiotsTogetherRateLimiter::TryAccept(double Now, double MinIntervalSeconds)
{
	if (bHasAccepted && (Now - LastAcceptedTime) < MinIntervalSeconds)
	{
		return false;
	}

	bHasAccepted = true;
	LastAcceptedTime = Now;
	return true;
}
