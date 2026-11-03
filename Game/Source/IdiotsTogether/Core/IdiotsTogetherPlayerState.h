#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "IdiotsTogetherTypes.h"
#include "IdiotsTogetherPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterTypeChanged, EIdiotsTogetherCharacterType, NewCharacterType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerColorIndexChanged, int32, NewColorIndex);

UCLASS()
class IDIOTSTOGETHER_API AIdiotsTogetherPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AIdiotsTogetherPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Idiots Together|Player")
	bool bIsReady;

	/** Index into UIdiotsTogetherSettings::PlayerColorPalette. Unique per player; assigned by the server on login. INDEX_NONE until then. */
	UPROPERTY(ReplicatedUsing = OnRep_PlayerColorIndex, BlueprintReadOnly, Category = "Idiots Together|Player")
	int32 PlayerColorIndex;

	/** Cosmetic identity. Duplicates are allowed (design decision (a)); colour tells players apart. */
	UPROPERTY(ReplicatedUsing = OnRep_CharacterType, BlueprintReadOnly, Category = "Idiots Together|Player")
	EIdiotsTogetherCharacterType CharacterType;

	UPROPERTY(BlueprintAssignable, Category = "Idiots Together|Player")
	FOnCharacterTypeChanged OnCharacterTypeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Idiots Together|Player")
	FOnPlayerColorIndexChanged OnPlayerColorIndexChanged;

	UFUNCTION()
	void OnRep_CharacterType();

	UFUNCTION()
	void OnRep_PlayerColorIndex();

	/**
	 * Server only. Applies a character request: a selectable identity is always accepted
	 * (duplicates allowed); Random resolves to the identity least used by the other players.
	 * Returns false for invalid requests. Clients go through
	 * AIdiotsTogetherPlayerController::ServerRequestCharacterSelection.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Idiots Together|Player")
	bool SelectCharacter_Server(EIdiotsTogetherCharacterType RequestedType);

	/** Server only. */
	void SetPlayerColorIndex_Server(int32 NewColorIndex);

	UFUNCTION(BlueprintPure, Category = "Idiots Together|Player")
	EIdiotsTogetherCharacterType GetCharacterType() const { return CharacterType; }

	UFUNCTION(BlueprintPure, Category = "Idiots Together|Player")
	int32 GetPlayerColorIndex() const { return PlayerColorIndex; }

	/** Resolved colour from the project palette (grey while unassigned). */
	UFUNCTION(BlueprintPure, Category = "Idiots Together|Player")
	FLinearColor GetPlayerColor() const;
};
