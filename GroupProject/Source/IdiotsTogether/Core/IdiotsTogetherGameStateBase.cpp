#include "IdiotsTogetherGameStateBase.h"
#include "Net/UnrealNetwork.h"

AIdiotsTogetherGameStateBase::AIdiotsTogetherGameStateBase()
{
	CurrentPhase = EScenarioPhase::Lobby;
	ActiveScenarioName = TEXT("Lobby");
}

void AIdiotsTogetherGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIdiotsTogetherGameStateBase, CurrentPhase);
	DOREPLIFETIME(AIdiotsTogetherGameStateBase, ActiveScenarioName);
}
