// Copyright (c) 2019-2020 Temporal Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NeatEntityEntry.generated.h"

/**
 * 
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
