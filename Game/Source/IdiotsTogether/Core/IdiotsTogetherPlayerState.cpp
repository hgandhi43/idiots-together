#include "IdiotsTogetherPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"
#include "Algo/RandomShuffle.h"
#include "IdiotsTogether/IdiotsTogether.h"

AIdiotsTogetherPlayerState::AIdiotsTogetherPlayerState()
{
	bIsReady = false;
	PlayerColorIndex = 0;
	CharacterType = EIdiotsTogetherCharacterType::None;
}

void AIdiotsTogetherPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIdiotsTogetherPlayerState, bIsReady);
	DOREPLIFETIME(AIdiotsTogetherPlayerState, PlayerColorIndex);
	DOREPLIFETIME(AIdiotsTogetherPlayerState, CharacterType);
}

void AIdiotsTogetherPlayerState::OnRep_CharacterType()
{
	UE_LOG(LogIdiotsTogetherPlayer, Verbose, TEXT("OnRep character type: player %d (%s) -> %d"),
		GetPlayerId(), *GetPlayerName(), static_cast<int32>(CharacterType));

	OnCharacterTypeChanged.Broadcast(CharacterType);
}

void AIdiotsTogetherPlayerState::ServerSelectCharacter_Implementation(EIdiotsTogetherCharacterType RequestedType)
{
	if (!HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	AGameStateBase* GS = World->GetGameState<AGameStateBase>();

	// TEAM RANDOM SELECTION
	if (RequestedType == EIdiotsTogetherCharacterType::Random)
	{
		// Available unique characters (max 4 selectable unique types)
		TArray<EIdiotsTogetherCharacterType> AvailablePool = {
			EIdiotsTogetherCharacterType::Boss,
			EIdiotsTogetherCharacterType::Nerd,
			EIdiotsTogetherCharacterType::Muscle,
			EIdiotsTogetherCharacterType::Intern
		};

		// Fisher-Yates / Algo::RandomShuffle to randomize team assignment
		Algo::RandomShuffle(AvailablePool);

		if (GS)
		{
			int32 Index = 0;
			for (APlayerState* OtherPSBase : GS->PlayerArray)
			{
				if (AIdiotsTogetherPlayerState* PS = Cast<AIdiotsTogetherPlayerState>(OtherPSBase))
				{
					// Assign unique character from shuffled pool (wrapping around gracefully if player count > pool size)
					EIdiotsTogetherCharacterType Assigned = AvailablePool[Index % AvailablePool.Num()];
					PS->CharacterType = Assigned;
					PS->OnRep_CharacterType();
					UE_LOG(LogIdiotsTogetherPlayer, Log, TEXT("Team random: %s (%d) -> %d"),
						*PS->GetPlayerName(), PS->GetPlayerId(), static_cast<int32>(Assigned));
					Index++;
				}
			}
		}
		return;
	}

	// MANUAL SELECTION WITH SERVER-AUTHORITATIVE UNIQUENESS VALIDATION
	bool bIsAlreadyTaken = false;
	if (GS)
	{
		for (APlayerState* OtherPSBase : GS->PlayerArray)
		{
			if (AIdiotsTogetherPlayerState* OtherPS = Cast<AIdiotsTogetherPlayerState>(OtherPSBase))
			{
				if (OtherPS != this && OtherPS->GetCharacterType() == RequestedType)
				{
					bIsAlreadyTaken = true;
					UE_LOG(LogIdiotsTogetherPlayer, Log, TEXT("Selection of %d rejected: taken by %s (%d)"),
						static_cast<int32>(RequestedType), *OtherPS->GetPlayerName(), OtherPS->GetPlayerId());
					break;
				}
			}
		}
	}

	if (!bIsAlreadyTaken)
	{
		CharacterType = RequestedType;
		OnRep_CharacterType();
		UE_LOG(LogIdiotsTogetherPlayer, Log, TEXT("Selection accepted: %s (%d) -> %d"),
			*GetPlayerName(), GetPlayerId(), static_cast<int32>(RequestedType));
	}
}

bool AIdiotsTogetherPlayerState::ServerSelectCharacter_Validate(EIdiotsTogetherCharacterType RequestedType)
{
	return true;
}

