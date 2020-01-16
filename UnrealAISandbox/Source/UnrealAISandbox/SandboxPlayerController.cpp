// Fill out your copyright notice in the Description page of Project Settings.


#include "SandboxPlayerController.h"

void ASandboxPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;
}
