#include "IdiotsTogetherGameModeBase.h"
#include "IdiotsTogetherPlayerController.h"
#include "IdiotsTogetherGameStateBase.h"
#include "IdiotsTogetherPlayerState.h"
#include "IdiotsTogetherCharacter.h"
#include "IdiotsTogether/IdiotsTogether.h"

AIdiotsTogetherGameModeBase::AIdiotsTogetherGameModeBase()
{
	PlayerControllerClass = AIdiotsTogetherPlayerController::StaticClass();
	static ConstructorHelpers::FClassFinder<APlayerController> PCClass(TEXT("/Game/Core/Game/BP_IdiotsTogetherPlayerController.BP_IdiotsTogetherPlayerController_C"));
	if (PCClass.Succeeded())
	{
		PlayerControllerClass = PCClass.Class;
	}

	DefaultPawnClass = AIdiotsTogetherCharacter::StaticClass();
	static ConstructorHelpers::FClassFinder<APawn> PawnClass(TEXT("/Game/Core/Characters/BP_IdiotsTogetherCharacter.BP_IdiotsTogetherCharacter_C"));
	if (PawnClass.Succeeded())
	{
		DefaultPawnClass = PawnClass.Class;
	}

	GameStateClass = AIdiotsTogetherGameStateBase::StaticClass();
	PlayerStateClass = AIdiotsTogetherPlayerState::StaticClass();
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
