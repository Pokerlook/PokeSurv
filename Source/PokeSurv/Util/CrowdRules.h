#pragma once

#include "CoreMinimal.h"
#include "CrowdRules.generated.h"

/**
 * A/B/C 밀기 규칙.
 * 여기 있는 함수들은 컴포넌트/액터를 모르고 순수 데이터만 다룬다.
 * 나중에 매니저(Subsystem)로 옮길 때 이 파일은 그대로 재사용.
 */

UENUM(BlueprintType)
enum class ECrowdMoveType : uint8
{
	Chase   UMETA(DisplayName = "Chase (추적)"),   // 플레이어 추적
	Flow    UMETA(DisplayName = "Flow (직진)")     // 고정 방향 직진 + 플레이어 회피
};

USTRUCT(BlueprintType)
struct FCrowdTuning
{
	GENERATED_BODY()

	/** 이웃과의 겹침을 한 프레임에 몇 %씩 풀지. 낮을수록 말랑, 높을수록 단단 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd|Push", meta = (ClampMin = "0.05", ClampMax = "1.0"))
	float Stiffness = 0.5f;

	/** 이웃 밀림의 한 프레임 최대 이동량 (빽빽할 때 떨림 방지) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd|Push")
	float MaxCorrectionPerFrame = 15.f;

	/** A에게 밀릴 때 옆으로 비켜나는 정도. 0 = 정면으로 밀려남, 1 = 완전히 옆으로 갈라짐 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd|Hero", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HeroLateralBias = 0.7f;

	/** A에게 밀릴 때 한 프레임 최대 이동량. A의 이동량보다 커야 A가 겹쳐 들어가지 않음 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd|Hero")
	float HeroMaxCorrectionPerFrame = 40.f;

	/** B: 반지름 합 + 이 값 안으로 들어오면 A 쪽으로는 못 감 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd|Hero")
	float HeroContactPadding = 5.f;

	/** C: 진행 방향 앞 이 거리 안에 A가 있으면 회피 시작. 작을수록 늦게, 짧게 피함 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd|Avoid")
	float AvoidLookAhead = 250.f;
	
	/**
	+	 * C: (내 반지름 + A 반지름)에 더하는 여유 폭.
	+	 * 이 폭 안에 A가 있을 때만 피하고, 경로에서 많이 겹칠수록 많이 꺾음. 0이면 딱 스칠 만큼만 피함
	+	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd|Avoid")
	float AvoidMargin  = 15.f;

	/** C: 회피 조향 세기. 1이면 정면 충돌 직전에 최대 45도 정도 꺾음 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd|Avoid")
	float AvoidStrength = 0.8f;
};

namespace CrowdRules
{
	/** 3D → XY 2D */
	FORCEINLINE FVector2f To2D(const FVector& V)
	{
		return FVector2f((float)V.X, (float)V.Y);
	}

	/** XY 2D → 3D. Z는 Original에서 유지 */
	FORCEINLINE FVector To3D(const FVector2f& P, const FVector& Original)
	{
		return FVector(P.X, P.Y, Original.Z);
	}

	FORCEINLINE FVector2f ClampSize(const FVector2f& V, float Max)
	{
		const float S2 = V.SizeSquared();
		return (S2 > Max * Max) ? V * (Max / FMath::Sqrt(S2)) : V;
	}

	FORCEINLINE FVector2f MoveTowards(const FVector2f& Current, const FVector2f& Target, float MaxDelta)
	{
		const FVector2f D = Target - Current;
		const float L = D.Size();
		return (L <= MaxDelta || L < KINDA_SMALL_NUMBER) ? Target : Current + D * (MaxDelta / L);
	}

	/**
	 * 두 개체가 겹쳤을 때 Me가 떠안는 비율. [Me][Other]
	 * A는 여기 없음: A와 겹치면 항상 100% 상대가 비킨다 (HeroCorrection).
	 */
	FORCEINLINE float Share(ECrowdMoveType  Me, ECrowdMoveType  Other)
	{
		//                                  vs B   vs C
		static constexpr float Table[2][2] = {
			{ 0.5f, 0.9f },   // B: B끼리 반반, C에게는 밀림
			{ 0.1f, 0.5f }    // C: B를 밀고 감, C끼리 반반
		};
		return Table[(uint8)Me][(uint8)Other];
	}

	/** 이웃 하나와의 겹침 보정량 (Me가 움직일 양) */
	FORCEINLINE FVector2f PairCorrection(
		const FVector2f& MyPos, float MyR, ECrowdMoveType  MyType,
		const FVector2f& OtherPos, float OtherR, ECrowdMoveType  OtherType)
	{
		const FVector2f D = MyPos - OtherPos;
		const float R = MyR + OtherR;
		const float D2 = D.SizeSquared();
		if (D2 >= R * R || D2 < KINDA_SMALL_NUMBER)
		{
			return FVector2f::ZeroVector;
		}
		const float Dist = FMath::Sqrt(D2);
		return D * ((R - Dist) / Dist * Share(MyType, OtherType));
	}

	/** A와의 겹침: 100% 내가 비킴. A가 움직이는 중이면 A 진행 방향의 옆으로 갈라지듯 비킴 */
	inline FVector2f HeroCorrection(
		const FVector2f& MyPos, float MyR,
		const FVector2f& HeroPos, float HeroR, const FVector2f& HeroVel,
		float LateralBias)
	{
		const FVector2f D = MyPos - HeroPos;
		const float R = MyR + HeroR;
		const float D2 = D.SizeSquared();
		if (D2 >= R * R)
		{
			return FVector2f::ZeroVector;
		}

		const float Dist = FMath::Sqrt(D2);
		const FVector2f HeroFwd = HeroVel.GetSafeNormal();

		FVector2f Dir = (Dist > KINDA_SMALL_NUMBER) ? D / Dist : FVector2f(-HeroFwd.Y, HeroFwd.X);
		if (Dir.IsNearlyZero())
		{
			Dir = FVector2f(1.f, 0.f);
		}

		if (!HeroFwd.IsNearlyZero())
		{
			FVector2f Side(-HeroFwd.Y, HeroFwd.X);
			if (FVector2f::DotProduct(Side, Dir) < 0.f)
			{
				Side = -Side;
			}
			Dir = FMath::Lerp(Dir, Side, LateralBias).GetSafeNormal();
		}

		return Dir * (R - Dist);
	}

	/** B: A와 접촉 거리 안이면 A를 향한 속도 성분 제거 → A를 못 밀고 옆으로 미끄러짐 */
	inline FVector2f BlockTowardHero(
		const FVector2f& Vel, const FVector2f& MyPos, float MyR,
		const FVector2f& HeroPos, float HeroR, float Padding,
		bool& bOutBlocked)
	{
		bOutBlocked = false;
		const FVector2f ToHero = HeroPos - MyPos;
		const float Dist = ToHero.Size();
		if (Dist > MyR + HeroR + Padding || Dist < KINDA_SMALL_NUMBER)
		{
			return Vel;
		}

		const FVector2f N = ToHero / Dist;
		const float Toward = FVector2f::DotProduct(Vel, N);
		if (Toward <= 0.f)
		{
			return Vel;
		}

		bOutBlocked = true;
		return Vel - N * Toward;
	}
	
	/**
	+	 * C: 흐름 방향으로 가되, 진행 경로에 A가 걸리면 걸린 만큼만 옆으로 조향.
	+	 * - Width = 내 반지름 + A 반지름 + 여유. 이 폭 밖이면 회피 안 함
	+	 * - 경로 가장자리에 살짝 걸친 개체는 조금만, 정중앙은 많이 꺾음
	+	 * - A를 지나치면(Ahead < 0) 즉시 원래 흐름 방향으로 복귀
	+	 * 반환값은 정규화된 방향
	+	 */
	inline FVector2f FlowWithAvoid(
		const FVector2f& FlowDir, const FVector2f& MyPos, const FVector2f& HeroPos,
		float LookAhead, float Width, float Strength)
	{
		const FVector2f ToHero = HeroPos - MyPos;
		const float Ahead = FVector2f::DotProduct(ToHero, FlowDir);
		const FVector2f Lateral = ToHero - FlowDir * Ahead;
		const float LatDist = Lateral.Size();

		if (Ahead <= 0.f || Ahead > LookAhead || LatDist >= Width)
		{
			return FlowDir;
		}

		const FVector2f Away = (LatDist > KINDA_SMALL_NUMBER)
			? -Lateral / LatDist
			: FVector2f(-FlowDir.Y, FlowDir.X); // 정면이면 오른쪽으로

		const float Closeness = 1.f - Ahead / LookAhead;   // 가까울수록 1
		const float Overlap = 1.f - LatDist / Width;       // 경로 중앙에 가까울수록 1
		const float W = Closeness * Overlap * Strength;

		return (FlowDir + Away * W).GetSafeNormal();
	}
}
