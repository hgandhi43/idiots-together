#include "IdiotsTogetherScenarioDefinition.h"

UIdiotsTogetherScenarioDefinition::UIdiotsTogetherScenarioDefinition()
{
	ScenarioID = TEXT("TestScenario");
	DisplayName = NSLOCTEXT("Scenario", "TestScenarioDisplayName", "Test Scenario");
	Description = NSLOCTEXT("Scenario", "TestScenarioDesc", "Default M3B Test Scenario for lifecycle verification.");
	BriefingDuration = 5.0f;
	PlanningDuration = 5.0f;
	ExecutionDuration = 10.0f;
	ResultsDuration = 5.0f;
	TargetRoundCount = 3;
}
