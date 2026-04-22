// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/UI/PSDamageTextWidget.h"

#include "Animation/UMGSequencePlayer.h"

void UPSDamageTextWidget::BindAnimationFinished(UUMGSequencePlayer* Player)
{
	if (!Player) return;

	Player->OnSequenceFinishedPlaying().Clear();
	
	Player->OnSequenceFinishedPlaying().AddLambda([this](UUMGSequencePlayer&)
	{
		OnDamageTextFinished.Broadcast(this);
	});
	
	/*TWeakObjectPtr<UDamageTextWidget> WeakThis(this);
	Player->OnSequenceFinishedPlaying().AddLambda([WeakThis](UUMGSequencePlayer&)
	{
		if (WeakThis.IsValid())
		{
			WeakThis->OnDamageTextFinished.Broadcast(WeakThis.Get());
		}
	});*/
}

void UPSDamageTextWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	 
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;
    
	FVector2D ScreenPos;
	PC->ProjectWorldLocationToScreen(WorldLocation, ScreenPos);
	SetPositionInViewport(ScreenPos);
}
