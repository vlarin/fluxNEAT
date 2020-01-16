// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include <flux/training/neat_activity_trainer.h>

#include "TrainingConsoleWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALAISANDBOX_API UTrainingConsoleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTrainingConsoleWidget(const FObjectInitializer& ObjectInitializer);

	// Optionally override the Blueprint "Event Construct" event
	virtual void NativeConstruct() override;

	// Optionally override the tick event
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	//Fire a Weapon
	UFUNCTION(BlueprintCallable, Category = "Handlers")
	void OnTrainingClick();

private:
	bool _isTraining;
	int _epoch;
	std::unique_ptr<flux::NeatActivityTrainer> _trainer;
	std::shared_ptr<flux::SingleActivityBlackBox> _blackBox;

	void TrainXOR();
};
