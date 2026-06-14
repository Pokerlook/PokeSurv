// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/UI/PSHUD.h"

#include "PSLevelUpMenuWidget.h"
#include "Blueprint/UserWidget.h"

TObjectPtr<UPSLevelUpMenuWidget> APSHUD::GetOrCreateLevelUpMenu()
{
	if (!LevelUpMenu)
	{
		if (!LevelUpMenuClass) return nullptr;
		LevelUpMenu = CreateWidget<UPSLevelUpMenuWidget>(GetWorld(), LevelUpMenuClass);
		if (!LevelUpMenu) return nullptr;
		LevelUpMenu->AddToViewport();
	}
	return LevelUpMenu;
}

void APSHUD::ShowLevelUpMenu(const TArray<FPSLevelUpOption>& Chosen) const
{
    if (!LevelUpMenu) return;
	
	LevelUpMenu->SetVisibility(ESlateVisibility::Visible);
	LevelUpMenu->SetupCards(Chosen);

	if (APlayerController* PC = GetOwningPlayerController())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(LevelUpMenu->TakeWidget());   // 메뉴에 포커스
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
	
}

void APSHUD::HideLevelUpMenu() const
{
    if (!LevelUpMenu) return;

	LevelUpMenu->SetVisibility(ESlateVisibility::Collapsed);

	if (APlayerController* PC = GetOwningPlayerController())
	{
		PC->SetInputMode(FInputModeGameOnly());   // 임시 객체 OK (게임 모드는 설정할 게 없음)
		PC->bShowMouseCursor = false;
	}
}

void APSHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		HUDWidget->AddToViewport();
	}
}
