#include "IdiotsTogetherPlayerState.h"
#include "Net/UnrealNetwork.h"

AIdiotsTogetherPlayerState::AIdiotsTogetherPlayerState()
{
	bIsReady = false;
	PlayerColorIndex = 0;
}

void AIdiotsTogetherPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIdiotsTogetherPlayerState, bIsReady);
	DOREPLIFETIME(AIdiotsTogetherPlayerState, PlayerColorIndex);
}
