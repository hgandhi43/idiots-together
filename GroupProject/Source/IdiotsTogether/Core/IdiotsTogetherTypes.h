#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
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

UENUM(BlueprintType)
enum class EIdiotsTogetherRoundPhase : uint8
{
	Waiting     UMETA(DisplayName = "Waiting"),
	Briefing    UMETA(DisplayName = "Briefing"),
	Planning    UMETA(DisplayName = "Planning"),
	Execution   UMETA(DisplayName = "Execution"),
	Success     UMETA(DisplayName = "Success"),
	Failure     UMETA(DisplayName = "Failure"),
	Results     UMETA(DisplayName = "Results"),
	NextRound   UMETA(DisplayName = "Next Round")
};

USTRUCT(BlueprintType)
struct FIdiotsTogetherObjectiveDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Objective")
	FString ObjectiveID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Objective")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Objective")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Objective")
	bool bIsRequired = true;
};

USTRUCT(BlueprintType)
struct FIdiotsTogetherObjectiveState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	FString ObjectiveID;

	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	bool bIsRequired = true;

	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	bool bIsCompleted = false;
};

inline FString GetNetworkRoleString(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return TEXT("UNKNOWN");
	}
	const UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return TEXT("UNKNOWN");
	}

	switch (World->GetNetMode())
	{
	case NM_DedicatedServer:
		return TEXT("SERVER");
	case NM_ListenServer:
		return TEXT("LISTEN SERVER");
	case NM_Client:
		return TEXT("CLIENT");
	case NM_Standalone:
		return TEXT("STANDALONE");
	default:
		return TEXT("UNKNOWN");
	}
}

inline const TCHAR* GetRoundPhaseString(EIdiotsTogetherRoundPhase Phase)
{
	switch (Phase)
	{
	case EIdiotsTogetherRoundPhase::Waiting:   return TEXT("Waiting");
	case EIdiotsTogetherRoundPhase::Briefing:  return TEXT("Briefing");
	case EIdiotsTogetherRoundPhase::Planning:  return TEXT("Planning");
	case EIdiotsTogetherRoundPhase::Execution: return TEXT("Execution");
	case EIdiotsTogetherRoundPhase::Success:   return TEXT("Success");
	case EIdiotsTogetherRoundPhase::Failure:   return TEXT("Failure");
	case EIdiotsTogetherRoundPhase::Results:   return TEXT("Results");
	case EIdiotsTogetherRoundPhase::NextRound: return TEXT("NextRound");
	default:                                   return TEXT("Unknown");
	}
}


