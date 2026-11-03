#pragma once

#include "CoreMinimal.h"

/**
 * Pure (world-free) interaction checks shared by the client prompt and the server RPC,
 * so both agree on what is reachable. Unit-tested in Tests/.
 */
namespace IdiotsTogetherInteractionRules
{
	/**
	 * True if the closest point of TargetBounds is within MaxDistance of ViewLocation.
	 * Measuring to the bounds (not the actor origin) keeps large objects fair.
	 * An invalid box is treated as unreachable.
	 */
	IDIOTSTOGETHER_API bool IsWithinReach(const FVector& ViewLocation, const FBox& TargetBounds, float MaxDistance);

	/** Closest point on TargetBounds to ViewLocation (ViewLocation itself if inside). */
	IDIOTSTOGETHER_API FVector GetClosestPoint(const FVector& ViewLocation, const FBox& TargetBounds);
}

/** Simple server-side per-sender throttle: accepts at most one request per MinIntervalSeconds. */
struct IDIOTSTOGETHER_API FIdiotsTogetherRateLimiter
{
	/** Returns true and records Now if enough time passed since the last accepted request. */
	bool TryAccept(double Now, double MinIntervalSeconds);

	void Reset() { bHasAccepted = false; LastAcceptedTime = 0.0; }

private:
	bool bHasAccepted = false;
	double LastAcceptedTime = 0.0;
};
