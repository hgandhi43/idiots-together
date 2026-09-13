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

	// SubHeader / Plan Your Move
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
			GS->OnRoundPhaseChanged.AddDynamic(this, &UIdiotsTogetherPhaseHUDWidget::OnRoundPhaseChanged);
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

	// Only Briefing and Planning phases show the HUD widget
	if (Phase != EIdiotsTogetherRoundPhase::Briefing && Phase != EIdiotsTogetherRoundPhase::Planning)
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
	}
}
