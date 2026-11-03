#include "IdiotsTogetherPlayerState.h"
#include "IdiotsTogetherCharacterRules.h"
#include "IdiotsTogetherSettings.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/GameStateBase.h"
#include "IdiotsTogether/IdiotsTogether.h"

AIdiotsTogetherPlayerState::AIdiotsTogetherPlayerState()
{
	bIsReady = false;
	PlayerColorIndex = INDEX_NONE;
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

void AIdiotsTogetherPlayerState::OnRep_PlayerColorIndex()
{
	UE_LOG(LogIdiotsTogetherPlayer, Verbose, TEXT("OnRep colour index: player %d (%s) -> %d"),
		GetPlayerId(), *GetPlayerName(), PlayerColorIndex);

	OnPlayerColorIndexChanged.Broadcast(PlayerColorIndex);
}

FLinearColor AIdiotsTogetherPlayerState::GetPlayerColor() const
{
	return UIdiotsTogetherSettings::Get()->GetPlayerColor(PlayerColorIndex);
}

bool AIdiotsTogetherPlayerState::SelectCharacter_Server(EIdiotsTogetherCharacterType RequestedType)
{
	if (!HasAuthority())
	{
		return false;
	}

	if (!IdiotsTogetherCharacterRules::IsValidSelectionRequest(RequestedType))
	{
		UE_LOG(LogIdiotsTogetherPlayer, Warning, TEXT("Rejected invalid character request %d from %s"),
			static_cast<int32>(RequestedType), *GetPlayerName());
		return false;
	}

	EIdiotsTogetherCharacterType Resolved = RequestedType;

	if (RequestedType == EIdiotsTogetherCharacterType::Random)
	{
		TArray<EIdiotsTogetherCharacterType> OtherIdentities;
		if (const AGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState() : nullptr)
		{
			for (const APlayerState* OtherBase : GS->PlayerArray)
			{
				const AIdiotsTogetherPlayerState* Other = Cast<AIdiotsTogetherPlayerState>(OtherBase);
				if (Other && Other != this && IdiotsTogetherCharacterRules::IsSelectableIdentity(Other->CharacterType))
				{
					OtherIdentities.Add(Other->CharacterType);
				}
			}
		}

		// Phase 5 will replace this with the round's seeded stream.
		FRandomStream Stream(FMath::Rand());
		Resolved = IdiotsTogetherCharacterRules::PickLeastUsedIdentity(OtherIdentities, Stream);
	}

	if (CharacterType != Resolved)
	{
		CharacterType = Resolved;
		OnRep_CharacterType(); // Listen-server host does not receive OnRep; call it manually.
	}

	UE_LOG(LogIdiotsTogetherPlayer, Log, TEXT("%s is now identity %d (requested %d)"),
		*GetPlayerName(), static_cast<int32>(Resolved), static_cast<int32>(RequestedType));
	return true;
}

void AIdiotsTogetherPlayerState::SetPlayerColorIndex_Server(int32 NewColorIndex)
{
	if (!HasAuthority() || PlayerColorIndex == NewColorIndex)
	{
		return;
	}

	PlayerColorIndex = NewColorIndex;
	OnRep_PlayerColorIndex();

	UE_LOG(LogIdiotsTogetherPlayer, Log, TEXT("%s assigned colour index %d"), *GetPlayerName(), PlayerColorIndex);
}
