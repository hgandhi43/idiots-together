#pragma once

#include "CoreMinimal.h"

// General / module-level messages.
DECLARE_LOG_CATEGORY_EXTERN(LogIdiotsTogether, Log, All);

// Per-system categories. Toggle at runtime with e.g. "Log LogIdiotsTogetherRound Verbose".
DECLARE_LOG_CATEGORY_EXTERN(LogIdiotsTogetherRound, Log, All);       // Round lifecycle / phases (GameMode, GameState)
DECLARE_LOG_CATEGORY_EXTERN(LogIdiotsTogetherObjective, Log, All);   // Objective completion and triggers
DECLARE_LOG_CATEGORY_EXTERN(LogIdiotsTogetherInteraction, Log, All); // Interaction traces and server interaction RPCs
DECLARE_LOG_CATEGORY_EXTERN(LogIdiotsTogetherPlayer, Log, All);      // PlayerState, character identity, colours, pawn setup
DECLARE_LOG_CATEGORY_EXTERN(LogIdiotsTogetherUI, Log, All);          // Widgets / HUD
DECLARE_LOG_CATEGORY_EXTERN(LogIdiotsTogetherDev, Log, All);         // Dev/cheat commands (never in Shipping)
