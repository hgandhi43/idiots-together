#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "IdiotsTogetherGameStateBase.generated.h"

UENUM(BlueprintType)
enum class EScenarioPhase : uint8
{
	Lobby,
	Starting,
	InProgress,
	Ended
};

UCLASS()
class IDIOTSTOGETHER_API AIdiotsTogetherGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AIdiotsTogetherGameStateBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Idiots Together|Scenario")
	EScenarioPhase CurrentPhase;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Idiots Together|Scenario")
	FString ActiveScenarioName;
};
