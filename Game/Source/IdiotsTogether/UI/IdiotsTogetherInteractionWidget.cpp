#include "IdiotsTogetherInteractionWidget.h"
#include "Components/TextBlock.h"

void UIIdiotsTogetherInteractionWidget::SetPromptText(const FText& Text)
{
	if (PromptText)
	{
		PromptText->SetText(Text);
	}
}

void UIIdiotsTogetherInteractionWidget::ShowPrompt()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UIIdiotsTogetherInteractionWidget::HidePrompt()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
