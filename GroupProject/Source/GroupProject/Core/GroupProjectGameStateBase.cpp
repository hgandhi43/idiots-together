#include "GroupProjectGameStateBase.h"
#include "Net/UnrealNetwork.h"

AGroupProjectGameStateBase::AGroupProjectGameStateBase()
{
	CurrentPhase = EScenarioPhase::Lobby;
	ActiveScenarioName = TEXT("Lobby");
}

void AGroupProjectGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGroupProjectGameStateBase, CurrentPhase);
	DOREPLIFETIME(AGroupProjectGameStateBase, ActiveScenarioName);
}
