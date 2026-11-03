#include "IdiotsTogetherCharacterRules.h"
#include "Math/RandomStream.h"

namespace IdiotsTogetherCharacterRules
{
	const TArray<EIdiotsTogetherCharacterType>& GetSelectableIdentities()
	{
		static const TArray<EIdiotsTogetherCharacterType> Identities = {
			EIdiotsTogetherCharacterType::Boss,
			EIdiotsTogetherCharacterType::Nerd,
			EIdiotsTogetherCharacterType::Muscle,
			EIdiotsTogetherCharacterType::Intern
		};
		return Identities;
	}

	bool IsSelectableIdentity(EIdiotsTogetherCharacterType Type)
	{
		return GetSelectableIdentities().Contains(Type);
	}

	bool IsValidSelectionRequest(EIdiotsTogetherCharacterType Type)
	{
		return IsSelectableIdentity(Type) || Type == EIdiotsTogetherCharacterType::Random;
	}

	EIdiotsTogetherCharacterType PickLeastUsedIdentity(
		const TArray<EIdiotsTogetherCharacterType>& OtherPlayersIdentities, FRandomStream& Stream)
	{
		const TArray<EIdiotsTogetherCharacterType>& Identities = GetSelectableIdentities();

		int32 LowestCount = MAX_int32;
		TArray<EIdiotsTogetherCharacterType, TInlineAllocator<4>> Candidates;

		for (const EIdiotsTogetherCharacterType Identity : Identities)
		{
			int32 Count = 0;
			for (const EIdiotsTogetherCharacterType Other : OtherPlayersIdentities)
			{
				if (Other == Identity)
				{
					++Count;
				}
			}

			if (Count < LowestCount)
			{
				LowestCount = Count;
				Candidates.Reset();
				Candidates.Add(Identity);
			}
			else if (Count == LowestCount)
			{
				Candidates.Add(Identity);
			}
		}

		check(Candidates.Num() > 0);
		return Candidates[Stream.RandRange(0, Candidates.Num() - 1)];
	}

	int32 PickColorIndex(const TArray<int32>& TakenIndices, int32 PaletteSize)
	{
		if (PaletteSize <= 0)
		{
			return INDEX_NONE;
		}

		int32 BestIndex = 0;
		int32 BestCount = MAX_int32;

		for (int32 Index = 0; Index < PaletteSize; ++Index)
		{
			int32 Count = 0;
			for (const int32 Taken : TakenIndices)
			{
				if (Taken == Index)
				{
					++Count;
				}
			}

			if (Count == 0)
			{
				return Index;
			}

			if (Count < BestCount)
			{
				BestCount = Count;
				BestIndex = Index;
			}
		}

		return BestIndex;
	}
}
