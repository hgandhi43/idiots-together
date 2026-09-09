#include "GroupProjectPlayerController.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "GroupProject/GroupProject.h"

AGroupProjectPlayerController::AGroupProjectPlayerController()
{
	bShowMouseCursor = false;
	DefaultMouseCursor = EMouseCursor::Default;
}

void AGroupProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		UE_LOG(LogGroupProject, Log, TEXT("Enhanced Input Subsystem initialized for PlayerController."));
	}
}

void AGroupProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}
