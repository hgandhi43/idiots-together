#pragma once

#include "Misc/AutomationTest.h"

/**
 * Shared settings for IdiotsTogether automation tests.
 * Naming: "IdiotsTogether.<Area>.<System>.<Case>"; run all with
 *   Automation RunTests IdiotsTogether
 * (see Scripts/test.bat). Rules tests are pure logic and need no level.
 */
#define IDIOTSTOGETHER_TEST_FLAGS (EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
