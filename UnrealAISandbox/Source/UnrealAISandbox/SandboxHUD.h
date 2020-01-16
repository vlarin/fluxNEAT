// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SandboxHUD.generated.h"

/**
 * 
 */
UCLASS()
class UNREALAISANDBOX_API ASandboxHUD : public AHUD
{
	GENERATED_BODY()

protected:
	void PostInitializeComponents() override;
	
};
