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

	UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] PlayerController BeginPlay: Name=%s, Class=%s, IsLocal=%s, PossessedPawn=%s (Class=%s)"),
		*GetName(),
		*GetClass()->GetName(),
		IsLocalController() ? TEXT("TRUE") : TEXT("FALSE"),
		GetPawn() ? *GetPawn()->GetName() : TEXT("NULL"),
		GetPawn() ? *GetPawn()->GetClass()->GetName() : TEXT("NONE"));

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] Enhanced Input Subsystem initialized for PlayerController."));
	}

	if (IsLocalController())
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
		UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] PlayerController SetInputMode FInputModeGameOnly enabled."));
	}

	if (IsLocalController() && InteractionWidgetClass)
	{
		InteractionWidgetInstance = CreateWidget<UGroupProjectInteractionWidget>(this, InteractionWidgetClass);
		if (InteractionWidgetInstance)
		{
			InteractionWidgetInstance->AddToViewport(10);
			InteractionWidgetInstance->HidePrompt();
			UE_LOG(LogGroupProject, Warning, TEXT("[RUNTIME DIAGNOSTIC] InteractionWidgetInstance created and added to viewport."));
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
