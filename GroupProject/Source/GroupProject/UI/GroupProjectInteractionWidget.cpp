#include "GroupProjectInteractionWidget.h"
#include "Components/TextBlock.h"

void UGroupProjectInteractionWidget::SetPromptText(const FText& Text)
{
	if (PromptText)
	{
		PromptText->SetText(Text);
	}
}

void UGroupProjectInteractionWidget::ShowPrompt()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UGroupProjectInteractionWidget::HidePrompt()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
