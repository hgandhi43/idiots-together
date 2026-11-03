#include "IdiotsTogetherPhaseHUDWidget.h"
#include "IdiotsTogether/Core/IdiotsTogetherGameStateBase.h"
#include "IdiotsTogether/Core/IdiotsTogetherPlayerController.h"
#include "IdiotsTogether/IdiotsTogether.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"
#include "Styling/CoreStyle.h"

TSharedRef<SWidget> UIdiotsTogetherPhaseHUDWidget::RebuildWidget()
{
	TSharedRef<SVerticalBox> MainBox = SNew(SVerticalBox);

	// Header / Divider Top
	MainBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.Padding(2.0f)
		[
			SAssignNew(HeaderTextBlock, STextBlock)
			.Text(FText::FromString(TEXT("=========================\n       BRIEFING\n=========================")))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
			.ColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.85f, 0.2f)))
			.Justification(ETextJustify::Center)
		];

	// Scenario Name
	MainBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.Padding(6.0f, 4.0f)
		[
			SAssignNew(ScenarioNameTextBlock, STextBlock)
			.Text(FText::FromString(TEXT("Test Scenario")))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
			.ColorAndOpacity(FSlateColor(FLinearColor::White))
			.Justification(ETextJustify::Center)
		];

	// Scenario Description
	MainBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.Padding(6.0f, 2.0f)
		[
			SAssignNew(ScenarioDescTextBlock, STextBlock)
			.Text(FText::FromString(TEXT("Default M3B Test Scenario for lifecycle verification.")))
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.85f, 0.85f, 0.85f)))
			.Justification(ETextJustify::Center)
			.WrapTextAt(440.0f)
		];

	// SubHeader / Plan Your Move / Objectives Header
	MainBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.Padding(6.0f, 4.0f)
		[
			SAssignNew(SubHeaderTextBlock, STextBlock)
			.Text(FText::GetEmpty())
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 13))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.8f, 1.0f)))
			.Justification(ETextJustify::Center)
		];

	// Countdown
	MainBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.Padding(6.0f, 4.0f)
		[
			SAssignNew(CountdownTextBlock, STextBlock)
			.Text(FText::FromString(TEXT("Starting in: 5")))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
			.ColorAndOpacity(FSlateColor(FLinearColor::Yellow))
			.Justification(ETextJustify::Center)
		];

	// Objectives List Box
	MainBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.Padding(6.0f, 4.0f)
		[
			SAssignNew(ObjectivesBox, SVerticalBox)
		];

	// Footer / Team instructions
	MainBox->AddSlot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.Padding(6.0f, 2.0f)
		[
			SAssignNew(FooterTextBlock, STextBlock)
			.Text(FText::GetEmpty())
			.Font(FCoreStyle::GetDefaultFontStyle("Italic", 10))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.7f, 0.7f, 0.7f)))
			.Justification(ETextJustify::Center)
		];

	TSharedRef<SBorder> BorderContainer = SNew(SBorder)
		.Padding(FMargin(16.0f))
		.BorderImage(FCoreStyle::Get().GetBrush("Border"))
		.BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.8f))
		.Visibility(EVisibility::HitTestInvisible)
		[
			MainBox
		];

	TSharedRef<SOverlay> Overlay = SNew(SOverlay)
		.Visibility(EVisibility::HitTestInvisible)
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		.Padding(FMargin(0.0f, 40.0f, 0.0f, 0.0f))
		[
			SNew(SBox)
			.WidthOverride(480.0f)
			[
				BorderContainer
			]
		];

	RootContainer = Overlay;
	return Overlay;
}

void UIdiotsTogetherPhaseHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (UWorld* World = GetWorld())
	{
		if (AIdiotsTogetherGameStateBase* GS = World->GetGameState<AIdiotsTogetherGameStateBase>())
		{
			GS->OnRoundPhaseChanged.RemoveDynamic(this, &UIdiotsTogetherPhaseHUDWidget::OnRoundPhaseChanged);
			GS->OnRoundPhaseChanged.AddDynamic(this, &UIdiotsTogetherPhaseHUDWidget::OnRoundPhaseChanged);

			GS->OnObjectiveStatesChanged.RemoveDynamic(this, &UIdiotsTogetherPhaseHUDWidget::OnObjectiveStatesChanged);
			GS->OnObjectiveStatesChanged.AddDynamic(this, &UIdiotsTogetherPhaseHUDWidget::OnObjectiveStatesChanged);
		}
	}

	UpdateHUD();
}

void UIdiotsTogetherPhaseHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateHUD();
}

void UIdiotsTogetherPhaseHUDWidget::OnRoundPhaseChanged(EIdiotsTogetherRoundPhase NewPhase, EIdiotsTogetherRoundPhase OldPhase)
{
	UpdateHUD();
}

void UIdiotsTogetherPhaseHUDWidget::OnObjectiveStatesChanged()
{
	UpdateHUD();
}

void UIdiotsTogetherPhaseHUDWidget::UpdateHUD()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	AIdiotsTogetherGameStateBase* GS = World->GetGameState<AIdiotsTogetherGameStateBase>();
	if (!GS)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	EIdiotsTogetherRoundPhase Phase = GS->CurrentRoundPhase;

	// Show HUD during Briefing, Planning, and Execution phases
	if (Phase != EIdiotsTogetherRoundPhase::Briefing &&
		Phase != EIdiotsTogetherRoundPhase::Planning &&
		Phase != EIdiotsTogetherRoundPhase::Execution)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);

	// Scenario Name & Description from replicated GameState properties
	FText NameText = GS->ScenarioDisplayName.IsEmpty() ? FText::FromString(TEXT("Test Scenario")) : GS->ScenarioDisplayName;
	FText DescText = GS->ScenarioDescription.IsEmpty() ? FText::FromString(TEXT("Default M3B Test Scenario for lifecycle verification.")) : GS->ScenarioDescription;

	if (ScenarioNameTextBlock.IsValid())
	{
		ScenarioNameTextBlock->SetText(NameText);
	}
	if (ScenarioDescTextBlock.IsValid())
	{
		ScenarioDescTextBlock->SetText(DescText);
	}

	int32 SecondsLeft = FMath::Max(0, FMath::CeilToInt(GS->RemainingPhaseTime));

	if (Phase == EIdiotsTogetherRoundPhase::Briefing)
	{
		if (HeaderTextBlock.IsValid())
		{
			HeaderTextBlock->SetText(FText::FromString(TEXT("=========================\n       BRIEFING\n=========================")));
		}
		if (SubHeaderTextBlock.IsValid())
		{
			SubHeaderTextBlock->SetText(FText::GetEmpty());
		}
		if (CountdownTextBlock.IsValid())
		{
			CountdownTextBlock->SetText(FText::FromString(FString::Printf(TEXT("Starting in: %d"), SecondsLeft)));
		}
		if (FooterTextBlock.IsValid())
		{
			FooterTextBlock->SetText(FText::GetEmpty());
		}
		if (ObjectivesBox.IsValid())
		{
			ObjectivesBox->ClearChildren();
		}
	}
	else if (Phase == EIdiotsTogetherRoundPhase::Planning)
	{
		if (HeaderTextBlock.IsValid())
		{
			HeaderTextBlock->SetText(FText::FromString(TEXT("=========================\n      PLANNING PHASE\n=========================")));
		}
		if (SubHeaderTextBlock.IsValid())
		{
			SubHeaderTextBlock->SetText(FText::FromString(TEXT("PLAN YOUR MOVE")));
		}
		if (CountdownTextBlock.IsValid())
		{
			CountdownTextBlock->SetText(FText::FromString(FString::Printf(TEXT("Planning ends in: %d"), SecondsLeft)));
		}
		if (FooterTextBlock.IsValid())
		{
			FooterTextBlock->SetText(FText::FromString(TEXT("Talk to your team and plan your approach.")));
		}
		if (ObjectivesBox.IsValid())
		{
			ObjectivesBox->ClearChildren();
		}
	}
	else if (Phase == EIdiotsTogetherRoundPhase::Execution)
	{
		if (HeaderTextBlock.IsValid())
		{
			HeaderTextBlock->SetText(FText::FromString(TEXT("=========================\n       EXECUTION\n=========================")));
		}
		if (SubHeaderTextBlock.IsValid())
		{
			SubHeaderTextBlock->SetText(FText::FromString(TEXT("OBJECTIVES")));
		}

		int32 Minutes = SecondsLeft / 60;
		int32 Seconds = SecondsLeft % 60;
		if (CountdownTextBlock.IsValid())
		{
			CountdownTextBlock->SetText(FText::FromString(FString::Printf(TEXT("Time: %02d:%02d"), Minutes, Seconds)));
		}

		if (FooterTextBlock.IsValid())
		{
			FooterTextBlock->SetText(FText::FromString(TEXT("Complete all required objectives before time runs out.")));
		}

		if (ObjectivesBox.IsValid())
		{
			ObjectivesBox->ClearChildren();

			for (const FIdiotsTogetherObjectiveState& ObjState : GS->ObjectiveStates)
			{
				FString CheckboxStr = ObjState.bIsCompleted ? TEXT("[X] ") : TEXT("[ ] ");
				FString NameStr = ObjState.DisplayName.IsEmpty() ? ObjState.ObjectiveID : ObjState.DisplayName.ToString();
				if (!ObjState.bIsRequired)
				{
					NameStr += TEXT(" (Optional)");
				}

				FText ObjText = FText::FromString(CheckboxStr + NameStr);
				FLinearColor TextColor = ObjState.bIsCompleted ?
					FLinearColor(0.2f, 1.0f, 0.3f, 1.0f) :
					(ObjState.bIsRequired ? FLinearColor::White : FLinearColor(0.7f, 0.7f, 0.7f, 1.0f));

				ObjectivesBox->AddSlot()
					.AutoHeight()
					.HAlign(HAlign_Left)
					.Padding(2.0f)
					[
						SNew(STextBlock)
						.Text(ObjText)
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
						.ColorAndOpacity(FSlateColor(TextColor))
					];
			}
		}
	}
}
