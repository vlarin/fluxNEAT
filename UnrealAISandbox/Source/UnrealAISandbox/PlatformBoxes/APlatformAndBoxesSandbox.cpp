//
// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.
//

#include "APlatformAndBoxesSandbox.h"

#include <Engine/World.h>
#include <Engine/StaticMesh.h>

// Sets default values
APlatformAndBoxesSandbox::APlatformAndBoxesSandbox()
{
	_isDead = false;
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

}

FVector APlatformAndBoxesSandbox::GetFallingCubeLocation() const
{
	if (_spawnedBoxes.Num() < 1)
	{
		const FVector actorLocation = Cast<UStaticMeshComponent>(_platform->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetComponentLocation();		
		return actorLocation;
	}
	
	const auto box = _spawnedBoxes.Last();
	return Cast<UStaticMeshComponent>(box->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetComponentLocation();
}

FVector APlatformAndBoxesSandbox::GetCentroidLocation() const
{
	const FVector actorLocation = Cast<UStaticMeshComponent>(_platform->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetComponentLocation();
	FVector centroid = actorLocation;
	return centroid;
	for (int i = 0; i < _spawnedBoxes.Num(); ++i)
	{
		if (i == _spawnedBoxes.Num() - 1)
		{
			continue;
		}
		
		auto box = _spawnedBoxes[i];
		const FVector location = Cast<UStaticMeshComponent>(box->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetComponentLocation();
		centroid += location;
	}

	return centroid / (_spawnedBoxes.Num() + 1.0);
}

FVector APlatformAndBoxesSandbox::GetPlatformSpeed() const
{
	auto platform = Cast<UStaticMeshComponent>(_platform->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	return platform->GetPhysicsLinearVelocity();
}

void APlatformAndBoxesSandbox::ApplyPlatformMovement(float dx, float dy)
{
	auto forceMultiplier = MovementAcceleration + ((_spawnedBoxes.Num() - 1) * 20);
	auto platform = Cast<UStaticMeshComponent>(_platform->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	platform->AddImpulse(FVector(dx * forceMultiplier, dy * forceMultiplier, 0));
}

void APlatformAndBoxesSandbox::CleanUp()
{
	for (auto box : _spawnedBoxes)
	{
		box->Destroy();
	}

	_platform->Destroy();
	_spawnedBoxes.Reset();
}

// Called when the game starts or when spawned
void APlatformAndBoxesSandbox::BeginPlay()
{
	Super::BeginPlay();

	InitializePlatform();
}

// Called every frame
void APlatformAndBoxesSandbox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsTesting && _isDead && _spawnedBoxes.Num() > 0)
	{
		for (auto box : _spawnedBoxes)
		{
			box->Destroy();
		}
		_spawnedBoxes.Reset();
	}

	if (!_isDead || IsTesting)
	{
		_fitness += DeltaTime;

		auto isAttemptCompleted = false;
		for (auto box : _spawnedBoxes)
		{
			if (Cast<UStaticMeshComponent>(box->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetComponentLocation().Z < GetActorLocation().Z)
			{
				if (_attempt >= EvaluationAttempts - 1 && !IsTesting)
				{
					_isDead = true;
					UE_LOG(LogActor, Warning, TEXT("Actor fitness is %f"), _fitness);
					return;
				}

				isAttemptCompleted = true;
				break;
			}
		}

		if (isAttemptCompleted)
		{
			for (auto box : _spawnedBoxes)
			{
				box->Destroy();
			}
			_lastPlatformSpawnTime = -000.1;
			_spawnedBoxes.Reset();
			InitializePlatform();
			_attempt++;
		}
		
		
		if (_lastPlatformSpawnTime <= 0 && BoxPrefab)
		{
			_lastPlatformSpawnTime += DeltaSpawnTime;

			UWorld* world = GetWorld();
			FActorSpawnParameters spawnParameters;
			spawnParameters.Owner = this;

			auto x = FMath::FRand() * 2;
			if ((_attempt + _spawnedBoxes.Num()) % 4 < 2) { x = -x; }
			if (x < 0) { x -= DeltaHeight * (1 - _spawnedBoxes.Num() / 10.0); } else { x += DeltaHeight * (1 - _spawnedBoxes.Num() / 10.0); }

			auto y = FMath::FRand() * 2;
			if ((_attempt + _spawnedBoxes.Num()) % 2 == 1) { y = -y; }
			if (y < 0) { y -= DeltaHeight * (1 - _spawnedBoxes.Num() / 10.0); } else { y += DeltaHeight * (1 - _spawnedBoxes.Num() / 10.0); }

			const FVector actorLocation = Cast<UStaticMeshComponent>(_platform->GetComponentByClass(UStaticMeshComponent::StaticClass()))->GetComponentLocation();
			auto nbox = world->SpawnActor<AActor>(BoxPrefab, actorLocation +
				FVector(x, y, _currentPlatformHeight),
				FRotator::ZeroRotator, spawnParameters);
			nbox->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			_currentPlatformHeight += DeltaHeight;
			_spawnedBoxes.Add(nbox);
		}

		_lastPlatformSpawnTime -= DeltaTime;
	}
}

void APlatformAndBoxesSandbox::InitializePlatform()
{
	if (_platform)
	{
		_platform->Destroy();
	}
	
	//test spawn
	UWorld* world = GetWorld();
	if (world && PlatformPrefab)
	{
		FActorSpawnParameters spawnParameters;
		spawnParameters.Owner = this;
		_platform = world->SpawnActor<AActor>(PlatformPrefab, GetActorLocation(), FRotator::ZeroRotator, spawnParameters);
		_platform->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	}
	else
	{
		UE_LOG(LogActor, Error, TEXT("Unable to spawn Platform!"));
	}

	_currentPlatformHeight = InitialHeight;
}

