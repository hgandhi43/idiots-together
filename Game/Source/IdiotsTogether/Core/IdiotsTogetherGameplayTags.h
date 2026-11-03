#pragma once

#include "NativeGameplayTags.h"

/**
 * Native gameplay tags that C++ refers to directly (defaults, tests, the built-in test scenario).
 * Designer-authored tags live in Config/Tags/IdiotsTogetherTags.ini under the same roots:
 *   Scenario.*   one per scenario (e.g. Scenario.TheCon)
 *   Objective.*  one per objective (e.g. Objective.TheCon.StealPainting)
 */
namespace IdiotsTogetherTags
{
	IDIOTSTOGETHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Scenario_Test_Foundation);
	IDIOTSTOGETHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Objective_Test_Primary);
	IDIOTSTOGETHER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Objective_Test_Optional);
}
