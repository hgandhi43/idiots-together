#include "IdiotsTogetherTestCommon.h"
#include "IdiotsTogether/Core/IdiotsTogetherCharacterRules.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	constexpr int32 CharacterRulesTest_NumSeeds = 200;
	constexpr int32 CharacterRulesTest_MaxPlayers = 6;

	/** Simulates players joining one by one and each picking Random. */
	TArray<EIdiotsTogetherCharacterType> CharacterRulesTest_AssignRandom(int32 PlayerCount, int32 Seed)
	{
		FRandomStream Stream(Seed);
		TArray<EIdiotsTogetherCharacterType> Assigned;
		for (int32 Player = 0; Player < PlayerCount; ++Player)
		{
			Assigned.Add(IdiotsTogetherCharacterRules::PickLeastUsedIdentity(Assigned, Stream));
		}
		return Assigned;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIdiotsTogetherCharacterSelectionValidityTest, "IdiotsTogether.Rules.Character.SelectionValidity", IDIOTSTOGETHER_TEST_FLAGS)
bool FIdiotsTogetherCharacterSelectionValidityTest::RunTest(const FString& Parameters)
{
	using namespace IdiotsTogetherCharacterRules;
	using EType = EIdiotsTogetherCharacterType;

	TestEqual(TEXT("Four selectable identities"), GetSelectableIdentities().Num(), 4);
	TestTrue(TEXT("Boss is selectable"), IsSelectableIdentity(EType::Boss));
	TestFalse(TEXT("None is not selectable"), IsSelectableIdentity(EType::None));
	TestFalse(TEXT("Random is not an identity"), IsSelectableIdentity(EType::Random));
	TestTrue(TEXT("Random is a valid request"), IsValidSelectionRequest(EType::Random));
	TestFalse(TEXT("None is not a valid request"), IsValidSelectionRequest(EType::None));
	TestFalse(TEXT("Out-of-range value is not a valid request"), IsValidSelectionRequest(static_cast<EType>(200)));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIdiotsTogetherCharacterRandomUniqueTest, "IdiotsTogether.Rules.Character.RandomUniqueUpToFour", IDIOTSTOGETHER_TEST_FLAGS)
bool FIdiotsTogetherCharacterRandomUniqueTest::RunTest(const FString& Parameters)
{
	for (int32 PlayerCount = 2; PlayerCount <= 4; ++PlayerCount)
	{
		for (int32 Seed = 0; Seed < CharacterRulesTest_NumSeeds; ++Seed)
		{
			const TArray<EIdiotsTogetherCharacterType> Assigned = CharacterRulesTest_AssignRandom(PlayerCount, Seed);
			TArray<EIdiotsTogetherCharacterType> Unique;
			for (const EIdiotsTogetherCharacterType Identity : Assigned)
			{
				Unique.AddUnique(Identity);
			}
			if (Unique.Num() != PlayerCount)
			{
				AddError(FString::Printf(TEXT("%d players, seed %d: duplicate identity assigned"), PlayerCount, Seed));
				return false;
			}
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIdiotsTogetherCharacterRandomBalancedTest, "IdiotsTogether.Rules.Character.RandomBalancedFiveSix", IDIOTSTOGETHER_TEST_FLAGS)
bool FIdiotsTogetherCharacterRandomBalancedTest::RunTest(const FString& Parameters)
{
	// Decision (a): duplicates are allowed for 5-6 players, but spread as evenly as possible.
	for (int32 PlayerCount = 5; PlayerCount <= CharacterRulesTest_MaxPlayers; ++PlayerCount)
	{
		for (int32 Seed = 0; Seed < CharacterRulesTest_NumSeeds; ++Seed)
		{
			const TArray<EIdiotsTogetherCharacterType> Assigned = CharacterRulesTest_AssignRandom(PlayerCount, Seed);

			int32 MinCount = MAX_int32;
			int32 MaxCount = 0;
			for (const EIdiotsTogetherCharacterType Identity : IdiotsTogetherCharacterRules::GetSelectableIdentities())
			{
				const int32 Count = Assigned.FilterByPredicate([Identity](EIdiotsTogetherCharacterType T) { return T == Identity; }).Num();
				MinCount = FMath::Min(MinCount, Count);
				MaxCount = FMath::Max(MaxCount, Count);
			}

			if (MaxCount - MinCount > 1)
			{
				AddError(FString::Printf(TEXT("%d players, seed %d: unbalanced identities (min %d, max %d)"), PlayerCount, Seed, MinCount, MaxCount));
				return false;
			}
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FIdiotsTogetherCharacterColorTest, "IdiotsTogether.Rules.Character.PlayerColors", IDIOTSTOGETHER_TEST_FLAGS)
bool FIdiotsTogetherCharacterColorTest::RunTest(const FString& Parameters)
{
	using namespace IdiotsTogetherCharacterRules;

	// Six players joining one by one get six distinct colours from a six-colour palette.
	TArray<int32> Taken;
	for (int32 Player = 0; Player < CharacterRulesTest_MaxPlayers; ++Player)
	{
		const int32 Picked = PickColorIndex(Taken, CharacterRulesTest_MaxPlayers);
		TestFalse(FString::Printf(TEXT("Player %d gets an unused colour"), Player), Taken.Contains(Picked));
		Taken.Add(Picked);
	}

	// A leaver frees their colour and the next joiner gets the lowest free slot.
	Taken.Remove(2);
	TestEqual(TEXT("Rejoiner takes the freed slot"), PickColorIndex(Taken, CharacterRulesTest_MaxPlayers), 2);

	// Palette exhausted: reuse the least-used slot instead of failing.
	TestEqual(TEXT("Full palette falls back to least-used (lowest) index"), PickColorIndex({ 0, 1, 1 }, 2), 0);

	TestEqual(TEXT("Empty palette yields INDEX_NONE"), PickColorIndex({}, 0), static_cast<int32>(INDEX_NONE));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
