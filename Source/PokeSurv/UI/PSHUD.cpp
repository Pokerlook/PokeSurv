// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/UI/PSHUD.h"

#include "Blueprint/UserWidget.h"

void APSHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		HUDWidget->AddToViewport();
	}
}
