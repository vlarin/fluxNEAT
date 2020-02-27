// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NeatEntityEntry.generated.h"

USTRUCT(BlueprintType)
struct FNeuralConnection
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D From;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D To;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Weight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsPositive;
};

USTRUCT(BlueprintType)
struct FNeuralMap
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FVector2D> InputNodes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FVector2D> HiddenNodes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FVector2D> OutputNodes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FNeuralConnection> Connections;	
};

/**
 * Represent blueprint-friendly representation of the entity
 */
UCLASS(BlueprintType)
class UNREALAISANDBOX_API UNeatEntityEntry : public UObject
{
	GENERATED_BODY()

public:
	UNeatEntityEntry() : UObject() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SpeciesId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Complexity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Fitness;
};
