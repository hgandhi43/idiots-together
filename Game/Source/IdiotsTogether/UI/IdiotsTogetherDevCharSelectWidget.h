#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IdiotsTogether/Core/IdiotsTogetherTypes.h"
#include "IdiotsTogetherDevCharSelectWidget.generated.h"

class STextBlock;

UCLASS()
class IDIOTSTOGETHER_API UIdiotsTogetherDevCharSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Idiots Together|UI")
	void SelectCharacter(EIdiotsTogetherCharacterType Selection);

	UFUNCTION(BlueprintCallable, Category = "Idiots Together|UI")
	void UpdateStatusText();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	TSharedPtr<STextBlock> StatusTextBlock;
};
