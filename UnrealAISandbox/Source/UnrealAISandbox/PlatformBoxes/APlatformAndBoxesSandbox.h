//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#pragma once

#include "Core.h"
#include "GameFramework/Actor.h"
#include "APlatformAndBoxesSandbox.generated.h"

UCLASS()
class UNREALAISANDBOX_API APlatformAndBoxesSandbox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlatformAndBoxesSandbox();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Sandbox Config")
	TSubclassOf<AActor> BoxPrefab;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Sandbox Config")
	TSubclassOf<AActor> PlatformPrefab;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Sandbox Config")
	float MovementAcceleration = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Sandbox Config")
	bool IsTesting = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Sandbox Config")
	float EvaluationAttempts = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Sandbox Config")
	float DeltaSpawnTime = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Sandbox Config")
	float DeltaHeight = 20;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Sandbox Config")
	float InitialHeight = 80;

	FVector GetFallingCubeLocation() const;
	FVector GetCentroidLocation() const;
	FVector GetPlatformSpeed() const;
	void ApplyPlatformMovement(float dx, float dy);
	void CleanUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsEvaluationCompleted() const { return _isDead; }
	double GetAggregatedFitness() const { return _fitness; };

private:
	UPROPERTY()
	AActor* _platform;

	UPROPERTY()
	TArray<AActor*> _spawnedBoxes;

	float _currentPlatformHeight;
	float _lastPlatformSpawnTime;
	float _fitness;
	bool _isDead;
	int _attempt;

	void InitializePlatform();
};
