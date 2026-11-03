#include "IdiotsTogetherInteractionWidget.h"
#include "Components/TextBlock.h"

void UIdiotsTogetherInteractionWidget::SetPromptText(const FText& Text)
{
	if (PromptText)
	{
		PromptText->SetText(Text);
	}
}

void UIdiotsTogetherInteractionWidget::ShowPrompt()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UIdiotsTogetherInteractionWidget::HidePrompt()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
