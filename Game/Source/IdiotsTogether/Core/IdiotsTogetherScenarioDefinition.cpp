#include "IdiotsTogetherScenarioDefinition.h"
#include "IdiotsTogetherGameplayTags.h"

UIdiotsTogetherScenarioDefinition::UIdiotsTogetherScenarioDefinition()
{
	// Defaults describe the built-in foundation test scenario, used when no asset is assigned.
	ScenarioTag = IdiotsTogetherTags::Scenario_Test_Foundation;
	DisplayName = NSLOCTEXT("Scenario", "TestScenarioDisplayName", "Gameplay Foundation Test");
	Description = NSLOCTEXT("Scenario", "TestScenarioDesc", "Test scenario for objectives and execution timing.");
	BriefingDuration = 5.0f;
	PlanningDuration = 5.0f;
	ExecutionDuration = 20.0f;
	ResultsDuration = 5.0f;
	TargetRoundCount = 3;

	FIdiotsTogetherObjectiveDefinition ReqObj;
	ReqObj.ObjectiveTag = IdiotsTogetherTags::Objective_Test_Primary;
	ReqObj.DisplayName = NSLOCTEXT("Scenario", "ReqObjName", "Complete Test Objective");
	ReqObj.Description = NSLOCTEXT("Scenario", "ReqObjDesc", "Interact with the test box, enter the trigger, or press 9 (dev).");
	ReqObj.bIsRequired = true;
	Objectives.Add(ReqObj);

	FIdiotsTogetherObjectiveDefinition OptObj;
	OptObj.ObjectiveTag = IdiotsTogetherTags::Objective_Test_Optional;
	OptObj.DisplayName = NSLOCTEXT("Scenario", "OptObjName", "Optional Test Objective");
	OptObj.Description = NSLOCTEXT("Scenario", "OptObjDesc", "Optional side task for testing.");
	OptObj.bIsRequired = false;
	Objectives.Add(OptObj);
}
