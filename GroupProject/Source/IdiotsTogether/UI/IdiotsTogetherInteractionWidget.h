#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IdiotsTogetherInteractionWidget.generated.h"

class UTextBlock;

UCLASS()
class IDIOTSTOGETHER_API UIIdiotsTogetherInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Idiots Together|UI")
	void SetPromptText(const FText& Text);

	UFUNCTION(BlueprintCallable, Category = "Idiots Together|UI")
	void ShowPrompt();

	UFUNCTION(BlueprintCallable, Category = "Idiots Together|UI")
	void HidePrompt();

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Idiots Together|UI")
	UTextBlock* PromptText;
};
