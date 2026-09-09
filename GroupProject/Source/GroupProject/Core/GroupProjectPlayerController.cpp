#include "GroupProjectPlayerController.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "GroupProject/UI/GroupProjectInteractionWidget.h"
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

	if (IsLocalController() && InteractionWidgetClass)
	{
		InteractionWidgetInstance = CreateWidget<UGroupProjectInteractionWidget>(this, InteractionWidgetClass);
		if (InteractionWidgetInstance)
		{
			InteractionWidgetInstance->AddToViewport(10);
			InteractionWidgetInstance->HidePrompt();
		}
	}
}

void AGroupProjectPlayerController::ShowInteractionPrompt(const FText& Text)
{
	if (InteractionWidgetInstance)
	{
		InteractionWidgetInstance->SetPromptText(Text);
		InteractionWidgetInstance->ShowPrompt();
	}
}

void AGroupProjectPlayerController::HideInteractionPrompt()
{
	if (InteractionWidgetInstance)
	{
		InteractionWidgetInstance->HidePrompt();
	}
}

void AGroupProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}
