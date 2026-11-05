// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Game/PSPoolSubsystem.h"

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

void UPSPoolSubsystem::InitPool(TSubclassOf<AActor> ActorClass, int32 PoolSize)
{
	if (!ActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[InitPool] ActorClass is null"));
		return;
	}
	
	if (!ActorClass->ImplementsInterface(UPSPoolable::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("[InitPool] %s does not implement IPSPoolable"),
			*ActorClass->GetName());
		return;
	}
    
	UE_LOG(LogTemp, Log, TEXT("[InitPool] Class: %s, PoolSize: %d"), *ActorClass->GetName(), PoolSize);
    
	if (PoolSize <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InitPool] PoolSize <= 0, pool will be empty"));
		return;
	}
	
	FPoolArray& PoolArray = ObjectPools.FindOrAdd(ActorClass);
	
	if (PoolArray.Num() > 0)  // FPoolArray에 Num() 추가 필요
	{
		UE_LOG(LogTemp, Error, TEXT("[InitPool] %s already has %d actors, adding %d more"),
			*ActorClass->GetName(), PoolArray.Num(), PoolSize);
	}
    
	for (int32 i = 0; i < PoolSize; i++)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        
		AActor* NewActor = GetWorld()->SpawnActor<AActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (NewActor)
		{
			IPSPoolable::Execute_OnReturnToPool(NewActor);
			PoolArray.Add(NewActor);
#if WITH_EDITOR
			 NewActor->SetFolderPath(FName(*FString::Printf(TEXT("Pool/%s"), *ActorClass->GetName())));
#endif
		}
	}
}

AActor* UPSPoolSubsystem::SpawnFromPool(TSubclassOf<AActor> ActorClass, FVector Location, FRotator Rotation)
{
	FPoolArray* PoolArray = ObjectPools.Find(ActorClass);
    
	if (!PoolArray)
	{
		UE_LOG(LogTemp, Error, TEXT("[SpawnFromPool] No pool for %s. InitPool 먼저 호출했는지 확인"),
			*ActorClass->GetName());
		return nullptr;
	}
	
	if (PoolArray->IsEmpty())	// 다 꺼내져서 풀이 비엇다
	{
		return nullptr;
	}
	
	AActor* Actor = PoolArray->Pop();
	if (Actor)
	{
		Actor->SetActorLocationAndRotation(Location, Rotation);
		IPSPoolable::Execute_OnSpawnFromPool(Actor);
		PoolArray->ActiveCount++; 
		
		if (IPSPoolable::Execute_ShouldCull(Actor))
		{
			CullTargets.Add(Actor);  // 컬링 대상만 추적
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
	FPoolArray* PoolArray = ObjectPools.Find(ActorClass);
	
	if (!PoolArray)
	{
		UE_LOG(LogTemp, Error, TEXT("[ReturnToPool] No pool for %s"), *ActorClass->GetName());
		return;
	}

	PoolArray->Add(Actor);
	PoolArray->ActiveCount = FMath::Max(0, PoolArray->ActiveCount - 1);	//ActiveCount--
	
	// ActivePoolables에서 제거 (컬링 추적 안 받았으면 RemoveSingleSwap이 그냥 0 반환)
	CullTargets.RemoveSingleSwap(Actor);
}

int32 UPSPoolSubsystem::GetActiveCount(TSubclassOf<AActor> ActorClass) const
{
	const FPoolArray* PoolArray = ObjectPools.Find(ActorClass);
	return PoolArray ? PoolArray->ActiveCount : 0;
}

void UPSPoolSubsystem::CullingCheck()
{
	UpdateScreenBounds();
    
	for (int32 i = CullTargets.Num() - 1; i >= 0; i--)
	{
		AActor* Actor = CullTargets[i].Get();
        
		if (!IsValid(Actor))
		{
			CullTargets.RemoveAtSwap(i);
			continue;
		}
        
		if (IPSPoolable::Execute_ShouldCull(Actor))
		{
			const FBox2D CullBounds = GetCullBounds();
			IPSPoolable::Execute_CheckCulling(Actor, CullBounds);
		}
	}
}

void UPSPoolSubsystem::UpdateScreenBounds()
{	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;
	
	int32 ViewX, ViewY;
	PC->GetViewportSize(ViewX, ViewY);
	if (ViewX <= 0 || ViewY <= 0) return;   // 첫 프레임 뷰포트 크기 0 가드
    
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
	
	if ((MaxX - MinX) < 10.f || (MaxY - MinY) < 10.f) return;	// 투영이 덜 된 경우 거르기
	
	ScreenWorldBounds = FBox2D(FVector2D(MinX, MinY), FVector2D(MaxX, MaxY));	// bIsValid = true
	
	//UE_LOG(LogTemp, Warning, TEXT("[Bounds] View:%dx%d  Extent:%s  Frame:%llu"), ViewX, ViewY, *ScreenWorldBounds.GetExtent().ToString(), GFrameCounter);
	
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
