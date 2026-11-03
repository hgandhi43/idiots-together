#include "IdiotsTogetherSettings.h"

UIdiotsTogetherSettings::UIdiotsTogetherSettings()
{
	// Okabe-Ito palette (sRGB), converted to linear by the FColor constructor.
	PlayerColorPalette = {
		FLinearColor(FColor(0xE6, 0x9F, 0x00)), // Orange
		FLinearColor(FColor(0x56, 0xB4, 0xE9)), // Sky blue
		FLinearColor(FColor(0x00, 0x9E, 0x73)), // Bluish green
		FLinearColor(FColor(0xF0, 0xE4, 0x42)), // Yellow
		FLinearColor(FColor(0x00, 0x72, 0xB2)), // Blue
		FLinearColor(FColor(0xD5, 0x5E, 0x00))  // Vermillion
	};

	UnassignedPlayerColor = FLinearColor(0.4f, 0.4f, 0.4f, 1.0f);

	InteractablePhases = { EIdiotsTogetherRoundPhase::Execution };

	bAllowClientDevCommands = false;
}

FLinearColor UIdiotsTogetherSettings::GetPlayerColor(int32 ColorIndex) const
{
	return PlayerColorPalette.IsValidIndex(ColorIndex) ? PlayerColorPalette[ColorIndex] : UnassignedPlayerColor;
}
