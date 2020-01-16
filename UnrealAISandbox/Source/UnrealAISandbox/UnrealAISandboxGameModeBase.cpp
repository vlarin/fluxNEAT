// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealAISandboxGameModeBase.h"
#include "SandboxHUD.h"
#include "UserWidget.h"

AUnrealAISandboxGameModeBase::AUnrealAISandboxGameModeBase()
{
	HUDClass = ASandboxHUD::StaticClass();
}

void AUnrealAISandboxGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	ChangeMenuWidget(StartingWidgetClass);
}

void AUnrealAISandboxGameModeBase::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}
	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}