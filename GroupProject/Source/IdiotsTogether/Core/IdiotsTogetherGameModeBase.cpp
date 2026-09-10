#include "IdiotsTogetherGameModeBase.h"
#include "IdiotsTogetherPlayerController.h"
#include "IdiotsTogetherGameStateBase.h"
#include "IdiotsTogetherPlayerState.h"
#include "IdiotsTogetherCharacter.h"
#include "IdiotsTogether/IdiotsTogether.h"

AIdiotsTogetherGameModeBase::AIdiotsTogetherGameModeBase()
{
	PlayerControllerClass = AIdiotsTogetherPlayerController::StaticClass();
	GameStateClass = AIdiotsTogetherGameStateBase::StaticClass();
	PlayerStateClass = AIdiotsTogetherPlayerState::StaticClass();
	DefaultPawnClass = AIdiotsTogetherCharacter::StaticClass();
}

void AIdiotsTogetherGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] GameMode BeginPlay: Class=%s, DefaultPawnClass=%s, PlayerControllerClass=%s"),
		*GetClass()->GetName(),
		DefaultPawnClass ? *DefaultPawnClass->GetName() : TEXT("NULL"),
		PlayerControllerClass ? *PlayerControllerClass->GetName() : TEXT("NULL"));
}

void AIdiotsTogetherGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogIdiotsTogether, Log, TEXT("Player Joined: %s"), *NewPlayer->GetName());
}

void AIdiotsTogetherGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (Exiting)
	{
		UE_LOG(LogIdiotsTogether, Log, TEXT("Player Left: %s"), *Exiting->GetName());
	}
}
