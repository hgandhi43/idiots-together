#include "IdiotsTogetherScenarioDefinition.h"

UIdiotsTogetherScenarioDefinition::UIdiotsTogetherScenarioDefinition()
{
	ScenarioID = TEXT("M3D_TEST");
	DisplayName = NSLOCTEXT("Scenario", "TestScenarioDisplayName", "Gameplay Foundation Test");
	Description = NSLOCTEXT("Scenario", "TestScenarioDesc", "M3D Gameplay Foundation test scenario for objectives and execution timing.");
	BriefingDuration = 5.0f;
	PlanningDuration = 5.0f;
	ExecutionDuration = 20.0f;
	ResultsDuration = 5.0f;
	TargetRoundCount = 3;

	FIdiotsTogetherObjectiveDefinition ReqObj;
	ReqObj.ObjectiveID = TEXT("TEST_OBJECTIVE_01");
	ReqObj.DisplayName = NSLOCTEXT("Scenario", "ReqObjName", "Complete Test Objective");
	ReqObj.Description = NSLOCTEXT("Scenario", "ReqObjDesc", "Interact with the test box, enter trigger, or press Key 9.");
	ReqObj.bIsRequired = true;
	Objectives.Add(ReqObj);

	FIdiotsTogetherObjectiveDefinition OptObj;
	OptObj.ObjectiveID = TEXT("TEST_OBJECTIVE_02");
	OptObj.DisplayName = NSLOCTEXT("Scenario", "OptObjName", "Optional Test Objective");
	OptObj.Description = NSLOCTEXT("Scenario", "OptObjDesc", "Optional side task for testing.");
	OptObj.bIsRequired = false;
	Objectives.Add(OptObj);
}
