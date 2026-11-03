#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "IdiotsTogetherTypes.h"
#include "IdiotsTogetherPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterTypeChanged, EIdiotsTogetherCharacterType, NewCharacterType);

UCLASS()
class IDIOTSTOGETHER_API AIdiotsTogetherPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AIdiotsTogetherPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Idiots Together|Player")
	bool bIsReady;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Idiots Together|Player")
	int32 PlayerColorIndex;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterType, BlueprintReadOnly, Category = "Idiots Together|Player")
	EIdiotsTogetherCharacterType CharacterType;

	UPROPERTY(BlueprintAssignable, Category = "Idiots Together|Player")
	FOnCharacterTypeChanged OnCharacterTypeChanged;

	UFUNCTION()
	void OnRep_CharacterType();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Idiots Together|Player")
	void ServerSelectCharacter(EIdiotsTogetherCharacterType RequestedType);

	EIdiotsTogetherCharacterType GetCharacterType() const { return CharacterType; }
};
