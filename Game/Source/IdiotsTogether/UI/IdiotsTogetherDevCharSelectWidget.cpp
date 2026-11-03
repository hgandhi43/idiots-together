#include "IdiotsTogetherDevCharSelectWidget.h"
#include "IdiotsTogether/Core/IdiotsTogetherPlayerController.h"
#include "IdiotsTogether/Core/IdiotsTogetherPlayerState.h"
#include "IdiotsTogether/IdiotsTogether.h"
#include "GameFramework/GameStateBase.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"
#include "Styling/CoreStyle.h"

TSharedRef<SWidget> UIdiotsTogetherDevCharSelectWidget::RebuildWidget()
{
	TSharedRef<SVerticalBox> Box = SNew(SVerticalBox);

	// Title
	Box->AddSlot()
		.AutoHeight()
		.Padding(4.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("[DEV TEST UI] CHARACTER SELECTION")))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
			.ColorAndOpacity(FSlateColor(FLinearColor::Yellow))
		];

	// Status text
	Box->AddSlot()
		.AutoHeight()
		.Padding(4.0f)
		[
			SAssignNew(StatusTextBlock, STextBlock)
			.Text(FText::FromString(TEXT("Current: UNSELECTED\nTeam: P1=None")))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 11))
			.ColorAndOpacity(FSlateColor(FLinearColor::White))
		];

	// Key shortcut hints
	Box->AddSlot()
		.AutoHeight()
		.Padding(4.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Keys: [1] Boss  [2] Nerd  [3] Muscle  [4] Intern  [5] Random (duplicates allowed)")))
			.Font(FCoreStyle::GetDefaultFontStyle("Italic", 10))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.85f, 0.85f, 0.85f)))
		];

	// Button row
	TSharedRef<SHorizontalBox> ButtonRow = SNew(SHorizontalBox);

	auto AddCharButton = [this, &ButtonRow](const FString& Label, EIdiotsTogetherCharacterType Type, const FLinearColor& Color)
	{
		ButtonRow->AddSlot()
			.AutoWidth()
			.Padding(2.0f)
			[
				SNew(SButton)
				.IsFocusable(false)
				.OnClicked_Lambda([this, Type]() {
					SelectCharacter(Type);
					return FReply::Handled();
				})
				.Content()
				[
					SNew(STextBlock)
					.Text(FText::FromString(Label))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
					.ColorAndOpacity(FSlateColor(Color))
				]
			];
	};

	AddCharButton(TEXT(" Boss "), EIdiotsTogetherCharacterType::Boss, FLinearColor(1.0f, 0.8f, 0.0f));
	AddCharButton(TEXT(" Nerd "), EIdiotsTogetherCharacterType::Nerd, FLinearColor(0.0f, 0.75f, 1.0f));
	AddCharButton(TEXT(" Muscle "), EIdiotsTogetherCharacterType::Muscle, FLinearColor(1.0f, 0.3f, 0.3f));
	AddCharButton(TEXT(" Intern "), EIdiotsTogetherCharacterType::Intern, FLinearColor(0.2f, 0.9f, 0.2f));
	AddCharButton(TEXT(" Random "), EIdiotsTogetherCharacterType::Random, FLinearColor(0.9f, 0.9f, 0.9f));

	Box->AddSlot()
		.AutoHeight()
		.Padding(2.0f)
		[
			ButtonRow
		];

	TSharedRef<SBorder> BorderContainer = SNew(SBorder)
		.Padding(FMargin(12.0f))
		.BorderImage(FCoreStyle::Get().GetBrush("Border"))
		.BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.75f))
		[
			Box
		];

	return SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(FMargin(20.0f, 20.0f, 0.0f, 0.0f))
		[
			SNew(SBox)
			.WidthOverride(460.0f)
			[
				BorderContainer
			]
		];
}

void UIdiotsTogetherDevCharSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateStatusText();
}

void UIdiotsTogetherDevCharSelectWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateStatusText();
}

void UIdiotsTogetherDevCharSelectWidget::SelectCharacter(EIdiotsTogetherCharacterType Selection)
{
	if (AIdiotsTogetherPlayerController* PC = Cast<AIdiotsTogetherPlayerController>(GetOwningPlayer()))
	{
		PC->ServerRequestCharacterSelection(Selection);
	}
}

void UIdiotsTogetherDevCharSelectWidget::UpdateStatusText()
{
	if (!StatusTextBlock.IsValid())
	{
		return;
	}

	FString LocalCharName = TEXT("UNSELECTED");
	FString TeamStatusStr;

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AIdiotsTogetherPlayerState* LocalPS = PC->GetPlayerState<AIdiotsTogetherPlayerState>())
		{
			LocalCharName = FString::Printf(TEXT("%s (colour %d)"),
				*UEnum::GetDisplayValueAsText(LocalPS->GetCharacterType()).ToString(), LocalPS->GetPlayerColorIndex());
		}

		if (UWorld* World = GetWorld())
		{
			if (AGameStateBase* GS = World->GetGameState<AGameStateBase>())
			{
				int32 PlayerIdx = 1;
				for (APlayerState* PSBase : GS->PlayerArray)
				{
					if (AIdiotsTogetherPlayerState* PS = Cast<AIdiotsTogetherPlayerState>(PSBase))
					{
						if (!TeamStatusStr.IsEmpty())
						{
							TeamStatusStr += TEXT(" | ");
						}
						TeamStatusStr += FString::Printf(TEXT("P%d=%s (c%d)"), PlayerIdx++,
							*UEnum::GetDisplayValueAsText(PS->GetCharacterType()).ToString(), PS->GetPlayerColorIndex());
					}
				}
			}
		}
	}

	StatusTextBlock->SetText(FText::FromString(FString::Printf(TEXT("Current: %s\nTeam: %s"), *LocalCharName, *TeamStatusStr)));
}
