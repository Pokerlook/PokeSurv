// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Game/PSPoolSubsystem.h"

#include "PokeSurv/Combat/Actor/PSPoolableActor.h"
#include "PokeSurv/Interface/PSPoolable.h"

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
}
