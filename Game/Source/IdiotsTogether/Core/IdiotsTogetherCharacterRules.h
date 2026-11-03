#pragma once

#include "CoreMinimal.h"
#include "IdiotsTogetherTypes.h"
#include "Math/RandomStream.h"

/**
 * Pure (world-free) rules for character identities and player colours.
 * Kept free of UObjects so they can be unit-tested without a level (see Tests/).
 *
 * Design decision (2026-11-03, option (a)): identities are cosmetic and MAY be duplicated.
 * Every player gets a unique colour instead, so 2-6 players always resolve.
 */
namespace IdiotsTogetherCharacterRules
{
	/** The identities a player can actually wear (excludes None and Random). */
	IDIOTSTOGETHER_API const TArray<EIdiotsTogetherCharacterType>& GetSelectableIdentities();

	/** True for Boss / Nerd / Muscle / Intern. */
	IDIOTSTOGETHER_API bool IsSelectableIdentity(EIdiotsTogetherCharacterType Type);

	/** True for anything a client may request: a selectable identity or Random. */
	IDIOTSTOGETHER_API bool IsValidSelectionRequest(EIdiotsTogetherCharacterType Type);

	/**
	 * Picks the identity that is least used among the other players. Ties are broken randomly
	 * with Stream. With 4 or fewer players this always yields a unique identity; with 5-6
	 * duplicates are spread as evenly as possible.
	 */
	IDIOTSTOGETHER_API EIdiotsTogetherCharacterType PickLeastUsedIdentity(
		const TArray<EIdiotsTogetherCharacterType>& OtherPlayersIdentities, FRandomStream& Stream);

	/**
	 * Picks a colour index for a joining player: the lowest index in [0, PaletteSize) not in
	 * TakenIndices. If every index is taken, returns the least-used index (lowest on ties).
	 * Returns INDEX_NONE when PaletteSize <= 0.
	 */
	IDIOTSTOGETHER_API int32 PickColorIndex(const TArray<int32>& TakenIndices, int32 PaletteSize);
}
