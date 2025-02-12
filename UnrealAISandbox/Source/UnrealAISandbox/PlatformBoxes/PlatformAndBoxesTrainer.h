// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformBoxes/APlatformAndBoxesSandbox.h"

#include <flux/common.h>
#include <flux/training.h>
#include "Common/NeatEntityEntry.h"
#include "ListView.h"

#include "PlatformAndBoxesTrainer.generated.h"

UENUM(BlueprintType)
enum TrainingMode
{
	TM_DISABLED,
	TM_TRAINING,
	TM_REVIEW
};

UCLASS()
class UNREALAISANDBOX_API APlatformAndBoxesTrainer : public AActor
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	APlatformAndBoxesTrainer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Trainer Bindings")
	UListView *EntitiesList;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Trainer Config")
	FString TestingEpoch  = "";
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Trainer Config")
	TSubclassOf<APlatformAndBoxesSandbox> SandboxPrefab;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Trainer Config")
	float TargetFitness = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Trainer Config")
	float PopulationSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Trainer Config")
	float SpeciesAmount = 2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Trainer Config")
	float SpawnGridSize = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Trainer Config")
	float ParallelEvaluationsAmount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Trainer Config")
	FString WorkingDirPath;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEAT Evolution Parameters")
	float weightMutationProb = 0.8;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEAT Evolution Parameters")
	float connAdditionProb = 0.505;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEAT Evolution Parameters")
	float connDeletionProb = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEAT Evolution Parameters")
	float biasMutationProb = 0.7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEAT Evolution Parameters")
	float nodeAdditionProb = 0.2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEAT Evolution Parameters")
	float bias = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEAT Evolution Parameters")
	float initialWeight = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEAT Evolution Parameters")
	float weightMutationSize = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEAT Evolution Parameters")
	float biasMutationSize = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEAT Evolution Parameters")
	float elitismProportion = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NEAT Evolution Parameters")
	bool isAcyclic = true;

	flux::NeatEvolutionParameters GetEvolutionParameters() const
	{
		return flux::NeatEvolutionParameters(weightMutationProb, connAdditionProb,
			connDeletionProb, biasMutationProb, nodeAdditionProb, bias, initialWeight,
			weightMutationSize, biasMutationSize, elitismProportion, isAcyclic);
	}

	UPROPERTY()
	TArray<APlatformAndBoxesSandbox*> CurrentEvaluatedSandboxes;

	UPROPERTY(BlueprintReadOnly)
	TArray<float> CurrentSpeciesDistribution;

	UPROPERTY(BlueprintReadOnly)
	FNeuralMap CurrentNeuralMap;
	
	UFUNCTION(BlueprintCallable)
	void ChangeTrainingMode(TrainingMode newMode);

	UFUNCTION(BlueprintCallable)
	void SelectNeuralMap(int Index);

	UFUNCTION(BlueprintCallable)
	int GetCurrentEpoch() const;
	UFUNCTION(BlueprintCallable)
	bool IsComplexifying() const;
	UFUNCTION(BlueprintCallable)
	float GetBestFitness() const;
	UFUNCTION(BlueprintCallable)
	float GetChampionFitness() const;
	UFUNCTION(BlueprintCallable)
	float GetMeanFitness() const;
	UFUNCTION(BlueprintCallable)
	float GetMeanComplexity() const;
	UFUNCTION(BlueprintCallable)
	float GetMeanEvaluationDuration() const;

	UFUNCTION(BlueprintCallable)
	float GetEvaluationPerSec() const;
	UFUNCTION(BlueprintCallable)
	float GetTotalEvaluations() const;
	UFUNCTION(BlueprintCallable)
	float GetTotalEvaluationTime() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	
	TrainingMode _trainingMode;
	std::shared_ptr<flux::NeatActivityUnit> _neatActivity;
	std::unique_ptr<flux::NeatActivityTrainer> _trainer;
	std::shared_ptr<flux::SingleActivityBlackBox> _blackBox;
	
	UPROPERTY()
	APlatformAndBoxesSandbox *_sandbox;

	UPROPERTY()
	TArray<UNeatEntityEntry*> _currentEntities;

};
