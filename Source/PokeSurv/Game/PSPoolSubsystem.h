// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PSPoolSubsystem.generated.h"

USTRUCT()
struct FPoolArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<AActor>> ObjectPool;
    int32 ActiveCount = 0;   // 활성화 된, 풀에서 나가있는 수

	bool IsEmpty() { return ObjectPool.IsEmpty(); }
	AActor* Pop() { return ObjectPool.Pop(); }
	void Add(AActor* Actor) { ObjectPool.Add(Actor); }
    int32 Num() const { return ObjectPool.Num(); }  
};

/**
 * 
 */
UCLASS()
class POKESURV_API UPSPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
    virtual void Deinitialize() override;
	
	void InitPool(TSubclassOf<AActor> ActorClass, int32 PoolSize); // 풀 초기화 - UPSSkillComponent::AddSkill 때 호출 & 
	AActor* SpawnFromPool(TSubclassOf<AActor> ActorClass, FVector Location, FRotator Rotation); // 풀에서 꺼내오기 - RequestProjectile 때 호출

	UFUNCTION(BlueprintCallable)
	void ReturnToPool(AActor* Actor); // 풀에 반환 -  poolable에서 사용

	int32 GetActiveCount(TSubclassOf<AActor> ActorClass) const;

	FBox2D GetScreenWorldBounds() const { return ScreenWorldBounds; }
	FBox2D GetCullBounds() const        { return ScreenWorldBounds.ExpandBy(CullMargin); }  // 컬링용
	
private:
	void CullingCheck();
    void UpdateScreenBounds();

protected:
	TMap<UClass*, FPoolArray> ObjectPools;
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> CullTargets;
	
	FTimerHandle CullingTimerHandle;
	static constexpr float CullingInterval = 0.1f;

	FBox2D ScreenWorldBounds{ForceInit};  // 화면 영역 (XY 평면)
    float CullMargin = 100.f;

};
