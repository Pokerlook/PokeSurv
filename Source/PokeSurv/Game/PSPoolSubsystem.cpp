// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Game/PSPoolSubsystem.h"

#include "PokeSurv/Actor/PSPoolableActor.h"
#include "PokeSurv/Interface/PSPoolable.h"

void UPSPoolSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	InWorld.GetTimerManager().SetTimer(
		CullingTimerHandle,
		this,
		&UPSPoolSubsystem::CullingCheck,
		CullingInterval,
		true  // Loop
	);
}

void UPSPoolSubsystem::Deinitialize()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CullingTimerHandle);
	}
	Super::Deinitialize();
}

void UPSPoolSubsystem::InitPool(TSubclassOf<APSPoolableActor> ActorClass, int32 PoolSize)
{
	FPoolArray& PoolArray = ObjectPools.FindOrAdd(ActorClass);
    
	for (int32 i = 0; i < PoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
		APSPoolableActor* NewActor = GetWorld()->SpawnActor<APSPoolableActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (NewActor)
		{
			IPSPoolable::Execute_OnReturnToPool(NewActor);
			PoolArray.Add(NewActor);
		}
	}
}

AActor* UPSPoolSubsystem::SpawnFromPool(TSubclassOf<APSPoolableActor> ActorClass, FVector Location, FRotator Rotation)
{
	FPoolArray& PoolArray = ObjectPools.FindOrAdd(ActorClass);
    
	if (PoolArray.IsEmpty())
	{
		// 풀 비었으면 nullptr 반환, 스킬에서 알아서 대기
		return nullptr;
	}

	AActor* Actor = PoolArray.Pop();
	if (Actor)
	{
		Actor->SetActorLocationAndRotation(Location, Rotation);
		IPSPoolable::Execute_OnSpawnFromPool(Actor);
		
		if (IPSPoolable::Execute_ShouldCull(Actor))
		{
			ActivePoolables.Add(Actor);  // 컬링 대상만 추적
		}

	}
	return Actor;
}

void UPSPoolSubsystem::ReturnToPool(AActor* Actor)
{
	if (!Actor) return;
    
	UClass* ActorClass = Actor->GetClass();
	checkf(ActorClass->ImplementsInterface(UPSPoolable::StaticClass()), 
		TEXT("ReturnToPool: %s is not Poolable"), *Actor->GetName());

	IPSPoolable::Execute_OnReturnToPool(Actor);
	FPoolArray& PoolArray = ObjectPools.FindOrAdd(ActorClass);
	PoolArray.Add(Actor);
	
	// ActivePoolables에서 제거 (컬링 추적 안 받았으면 RemoveSingleSwap이 그냥 0 반환)
	ActivePoolables.RemoveSingleSwap(Actor);
}

void UPSPoolSubsystem::CullingCheck()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController(); !PC) return;
	
	UpdateScreenBounds();
    
	for (int32 i = ActivePoolables.Num() - 1; i >= 0; i--)
	{
		AActor* Actor = ActivePoolables[i].Get();
        
		if (!IsValid(Actor))
		{
			ActivePoolables.RemoveAtSwap(i);
			continue;
		}
        
		if (IPSPoolable::Execute_ShouldCull(Actor))
		{
			IPSPoolable::Execute_CheckCulling(Actor, ScreenWorldBounds);
		}
	}
}

void UPSPoolSubsystem::UpdateScreenBounds()
{	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;
	
	int32 ViewX, ViewY;
	PC->GetViewportSize(ViewX, ViewY);
    
	// 화면 네 모서리를 월드 좌표로 변환
	FVector TopLeftWorld, TopLeftDir;
	FVector BottomRightWorld, BottomRightDir;
    
	PC->DeprojectScreenPositionToWorld(0.f, 0.f, TopLeftWorld, TopLeftDir);
	PC->DeprojectScreenPositionToWorld(static_cast<float>(ViewX), static_cast<float>(ViewY), 
									   BottomRightWorld, BottomRightDir);
	
	// 두 모서리에서 X, Y 최소/최대 추출 (회전 무관하게 작동)
	const float MinX = FMath::Min(TopLeftWorld.X, BottomRightWorld.X);
	const float MaxX = FMath::Max(TopLeftWorld.X, BottomRightWorld.X);
	const float MinY = FMath::Min(TopLeftWorld.Y, BottomRightWorld.Y);
	const float MaxY = FMath::Max(TopLeftWorld.Y, BottomRightWorld.Y);
    
	constexpr float Margin = 100.f;
    
	ScreenWorldBounds.Min = FVector2D(MinX - Margin, MinY - Margin);
	ScreenWorldBounds.Max = FVector2D(MaxX + Margin, MaxY + Margin);
	ScreenWorldBounds.bIsValid = true;
	
	// 디버그 박스 그리기
	/*
	const FVector BoxCenter(
		(ScreenWorldBounds.Min.X + ScreenWorldBounds.Max.X) * 0.5f,
		(ScreenWorldBounds.Min.Y + ScreenWorldBounds.Max.Y) * 0.5f,
		0.f
	);
	const FVector BoxExtent(
		(ScreenWorldBounds.Max.X - ScreenWorldBounds.Min.X) * 0.5f,
		(ScreenWorldBounds.Max.Y - ScreenWorldBounds.Min.Y) * 0.5f,
		10.f
	);
	DrawDebugBox(GetWorld(), BoxCenter, BoxExtent, FColor::Yellow, false, CullingInterval * 1.1f);
	*/
}
