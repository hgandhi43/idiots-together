#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IdiotsTogether/Core/IdiotsTogetherTypes.h"
#include "IdiotsTogetherPhaseHUDWidget.generated.h"

class STextBlock;
class SWidget;

UCLASS()
class IDIOTSTOGETHER_API UIdiotsTogetherPhaseHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Idiots Together|UI")
	void UpdateHUD();

	UFUNCTION()
	void OnRoundPhaseChanged(EIdiotsTogetherRoundPhase NewPhase, EIdiotsTogetherRoundPhase OldPhase);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	TSharedPtr<STextBlock> HeaderTextBlock;
	TSharedPtr<STextBlock> ScenarioNameTextBlock;
	TSharedPtr<STextBlock> ScenarioDescTextBlock;
	TSharedPtr<STextBlock> SubHeaderTextBlock;
	TSharedPtr<STextBlock> CountdownTextBlock;
	TSharedPtr<STextBlock> FooterTextBlock;
	TSharedPtr<SWidget> RootContainer;
};
