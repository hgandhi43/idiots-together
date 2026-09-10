#pragma once

#include "CoreMinimal.h"
#include "IdiotsTogetherTypes.generated.h"

UENUM(BlueprintType)
enum class EIdiotsTogetherCharacterType : uint8
{
	None        UMETA(DisplayName = "None / Unselected"),
	Boss        UMETA(DisplayName = "Boss"),
	Nerd        UMETA(DisplayName = "Nerd"),
	Muscle      UMETA(DisplayName = "Muscle"),
	Intern      UMETA(DisplayName = "Intern"),
	Random      UMETA(DisplayName = "Random")
};
