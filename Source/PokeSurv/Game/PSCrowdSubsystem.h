// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PokeSurv/Util/CrowdRules.h"
#include "Subsystems/WorldSubsystem.h"
#include "PSCrowdSubsystem.generated.h"

class UPSCrowdMovementComponent;
class APawn;

/**
 * 프레임 시작 시점의 에이전트 상태 사본.
 * 모든 에이전트가 같은 사본을 읽으므로 컴포넌트 틱 순서에 결과가 좌우되지 않음.
 * (SoA 배열)
 */
struct FCrowdAgentSnapshot
{
	const UPSCrowdMovementComponent* Owner = nullptr; // 자기 자신 제외용. 역참조하지 않음
	FVector2f Pos = FVector2f::ZeroVector;
	float Radius = 0.f;
	ECrowdMoveType Type = ECrowdMoveType::Chase;
};

struct FCrowdHeroInfo
{
	FVector2f Pos = FVector2f::ZeroVector;
	FVector2f Vel = FVector2f::ZeroVector;
	float Radius = 40.f;
};

/**
 * 실험 단계용 레지스트리: 이웃 검색(공간 해시)과 Hero 정보만 제공.
 * 이동/밀기 로직은 각 컴포넌트가 수행. (나중에 이 루프가 여기로 승격 예정)
 */
UCLASS()
class POKESURV_API UPSCrowdSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	void Register(UPSCrowdMovementComponent* Agent);
	void Unregister(UPSCrowdMovementComponent* Agent);

	bool GetHeroInfo(FCrowdHeroInfo& Out);
	
	/** Pos 주변 3x3 셀의 에이전트를 순회 (Self 제외). 그리드는 프레임당 한 번 자동 재구성 */
	template<typename FuncType>
	void ForEachNeighbor(const FVector2f& Pos, const UPSCrowdMovementComponent* Self, FuncType&& Func)
	{
		RebuildIfNeeded();
		const FIntPoint Center = ToCell(Pos);
		for (int32 DY = -1; DY <= 1; ++DY)
		{
			for (int32 DX = -1; DX <= 1; ++DX)
			{
				if (const TArray<int32>* Cell = Cells.Find(FIntPoint(Center.X + DX, Center.Y + DY)))
				{
					for (const int32 Idx : *Cell)
					{
						const FCrowdAgentSnapshot& S = Snapshots[Idx];
						if (S.Owner != Self)
						{
							Func(S);
						}
					}
				}
			}
		}
	}

	int32 GetNumAgents() const { return Agents.Num(); }
	
private:
	void RebuildIfNeeded();
	
	FIntPoint ToCell(const FVector2f& P) const
	{
		return FIntPoint(FMath::FloorToInt(P.X / CellSize), FMath::FloorToInt(P.Y / CellSize));
	}
	
protected:
	/** 셀 크기. 가장 큰 에이전트 지름 이상이어야 이웃을 놓치지 않음 */
	float CellSize = 100.f;

	UPROPERTY()
	TArray<TObjectPtr<UPSCrowdMovementComponent>> Agents;

	TWeakObjectPtr<APawn> CachedHero;
	float CachedHeroRadius = 40.f;

	TArray<FCrowdAgentSnapshot> Snapshots;
	TMap<FIntPoint, TArray<int32>> Cells;
	uint64 BuiltFrame = MAX_uint64;
};
