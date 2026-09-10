#include "GroupProjectGameModeBase.h"
#include "GroupProjectPlayerController.h"
#include "GroupProjectGameStateBase.h"
#include "GroupProjectPlayerState.h"
#include "GroupProjectCharacter.h"
#include "GroupProject/GroupProject.h"

AGroupProjectGameModeBase::AGroupProjectGameModeBase()
{
	PlayerControllerClass = AGroupProjectPlayerController::StaticClass();
	GameStateClass = AGroupProjectGameStateBase::StaticClass();
	PlayerStateClass = AGroupProjectPlayerState::StaticClass();
	DefaultPawnClass = AGroupProjectCharacter::StaticClass();
}

void AGroupProjectGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] GameMode BeginPlay: Class=%s, DefaultPawnClass=%s, PlayerControllerClass=%s"),
		*GetClass()->GetName(),
		DefaultPawnClass ? *DefaultPawnClass->GetName() : TEXT("NULL"),
		PlayerControllerClass ? *PlayerControllerClass->GetName() : TEXT("NULL"));
}

void AGroupProjectGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogGroupProject, Log, TEXT("Player Joined: %s"), *NewPlayer->GetName());
}

void AGroupProjectGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if (Exiting)
	{
		UE_LOG(LogGroupProject, Log, TEXT("Player Left: %s"), *Exiting->GetName());
	}
}
