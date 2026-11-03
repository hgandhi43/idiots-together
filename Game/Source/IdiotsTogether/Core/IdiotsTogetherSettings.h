#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "IdiotsTogetherTypes.h"
#include "IdiotsTogetherSettings.generated.h"

/**
 * Project-wide tunables, editable in Project Settings > Game > Idiots Together and saved to
 * Config/DefaultGame.ini. Read on both server and clients (class defaults), so values that
 * clients need for presentation (e.g. player colours) belong here rather than on the GameMode.
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Idiots Together"))
class IDIOTSTOGETHER_API UIdiotsTogetherSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UIdiotsTogetherSettings();

	static const UIdiotsTogetherSettings* Get() { return GetDefault<UIdiotsTogetherSettings>(); }

	/** Colour for a PlayerColorIndex; UnassignedPlayerColor when the index is out of range. */
	FLinearColor GetPlayerColor(int32 ColorIndex) const;

	virtual FName GetCategoryName() const override { return TEXT("Game"); }

	/**
	 * One colour per player slot. The server gives each player a unique index into this list.
	 * Defaults are the Okabe-Ito colourblind-safe palette. Keep at least 6 entries (max players).
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Players")
	TArray<FLinearColor> PlayerColorPalette;

	/** Used before the server has assigned a colour. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Players")
	FLinearColor UnassignedPlayerColor;

	/** Round phases in which players may interact with objects. Briefing/Planning are hands-off by design. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TArray<EIdiotsTogetherRoundPhase> InteractablePhases;

	/**
	 * Development builds only: let non-host clients run dev commands (phase skip, force result...).
	 * Off by default so dev commands are host-only. Has no effect in Shipping, where dev commands do not exist.
	 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Development")
	bool bAllowClientDevCommands;
};
