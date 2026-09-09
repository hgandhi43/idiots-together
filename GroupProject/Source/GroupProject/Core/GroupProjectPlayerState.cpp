#include "GroupProjectPlayerState.h"
#include "Net/UnrealNetwork.h"

AGroupProjectPlayerState::AGroupProjectPlayerState()
{
	bIsReady = false;
	PlayerColorIndex = 0;
}

void AGroupProjectPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGroupProjectPlayerState, bIsReady);
	DOREPLIFETIME(AGroupProjectPlayerState, PlayerColorIndex);
}
