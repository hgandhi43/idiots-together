#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
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
};
