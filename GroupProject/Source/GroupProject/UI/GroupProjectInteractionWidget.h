#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GroupProjectInteractionWidget.generated.h"

class UTextBlock;

UCLASS()
class GROUPPROJECT_API UGroupProjectInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Group Project|UI")
	void SetPromptText(const FText& Text);

	UFUNCTION(BlueprintCallable, Category = "Group Project|UI")
	void ShowPrompt();

	UFUNCTION(BlueprintCallable, Category = "Group Project|UI")
	void HidePrompt();

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Group Project|UI")
	UTextBlock* PromptText;
};
