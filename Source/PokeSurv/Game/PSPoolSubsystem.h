// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PSPoolSubsystem.generated.h"

class APSPoolableActor;

USTRUCT()
struct FPoolArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<AActor>> ObjectPool;

	bool IsEmpty() { return ObjectPool.IsEmpty(); }
	AActor* Pop() { return ObjectPool.Pop(); }
	void Add(AActor* Actor) { ObjectPool.Add(Actor); }
};

/**
 * 
 */
UCLASS()
class POKESURV_API UPSPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	void InitPool(TSubclassOf<APSPoolableActor> ActorClass, int32 PoolSize); // 풀 초기화 - UPSSkillComponent::AddSkill 때 호출 & 
	AActor* SpawnFromPool(TSubclassOf<APSPoolableActor> ActorClass, FVector Location, FRotator Rotation); // 풀에서 꺼내오기 - RequestProjectile 때 호출
	void ReturnToPool(AActor* Actor); // 풀에 반환 -  poolable에서 사용

private:
	TMap<UClass*, FPoolArray> ObjectPools;
};
