// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Combat/PSSkillComponent.h"

#include "PokeSurv/Combat/Item/PSItemBase.h"
#include "PokeSurv/Game/PSPoolSubsystem.h"
#include "PokeSurv/Util/PSLibrary.h"
#include "PokeSurv/Combat/Projectile/PSProjectileBase.h"
#include "PokeSurv/Combat/Skill/PSSkillBase.h"

UPSSkillComponent::UPSSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPSSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultSkill)
	{
		AddSkill(DefaultSkill);
	}
}

void UPSSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (UPSSkillBase* Skill : Skills)
	{
		if (Skill)
		{
			Skill->TickCooldown(DeltaTime);
		}
	}
}

void UPSSkillComponent::AddSkill(TSubclassOf<UPSSkillBase> SkillClass)
{
	if (!SkillClass) return;

	if (UPSSkillBase* Existing = FindSkill(SkillClass))
	{
		Existing->LevelUp();
		return;
	}
    
	UPSSkillBase* NewSkill = NewObject<UPSSkillBase>(this, SkillClass);
	
	UPSPoolSubsystem* PoolSubsystem = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	PoolSubsystem->InitPool(TSubclassOf<APSPoolableActor>(NewSkill->SkillData->ProjectileClass), NewSkill->SkillData->ProjectileLimit);
	
	NewSkill->InitSkill(GetOwner());
	Skills.Add(NewSkill);
}

void UPSSkillComponent::AddItem(TSubclassOf<UPSItemBase> ItemClass)
{
	if (UPSItemBase* Existing = FindItem(ItemClass))
	{
		Existing->LevelUp();
		return;
	}

    // 없으면 추가 
    UPSItemBase* NewItem = NewObject<UPSItemBase>(this, ItemClass);
    NewItem->InitItem(GetOwner());
    Items.Add(NewItem);
}

UPSSkillBase* UPSSkillComponent::FindSkill(const TSubclassOf<UPSSkillBase>& SkillClass) const
{
	if (!SkillClass) return nullptr;
	for (UPSSkillBase* Skill : Skills)
	{
		if (Skill && Skill->GetClass() == SkillClass)
		{
			return Skill;
		}
	}
	return nullptr;
}

UPSItemBase* UPSSkillComponent::FindItem(const TSubclassOf<UPSItemBase>& ItemClass) const
{
	if (!ItemClass) return nullptr;
	for (UPSItemBase* Item : Items)
	{
		if (Item && Item->GetClass() == ItemClass)
		{
			return Item;
		}
	}
	return nullptr;
}

bool UPSSkillComponent::IsOptionNew(const FPSLevelUpOption& Option) const
{
	return Option.bIsSkill
		? (FindSkill(Option.SkillClass) == nullptr)
		: (FindItem(Option.ItemClass) == nullptr);
}

int32 UPSSkillComponent::GetOptionCurrentLevel(const FPSLevelUpOption& Option) const
{
	if (Option.bIsSkill)
	{
		const UPSSkillBase* S = FindSkill(Option.SkillClass);
		return S ? (S->SkillLevel + 1) : 0;
	}
	else
	{
		const UPSItemBase* I = FindItem(Option.ItemClass);
		return I ? (I->ItemLevel + 1) : 0;
	}
}

FText UPSSkillComponent::GetOptionNextLevelDescription(const FPSLevelUpOption& Option) const
{
	// 아이템이면 기본 설명
	if (!Option.bIsSkill)
	{
		return UPSLibrary::GetOptionBaseDescription(Option);
	}
	
	const UPSSkillBase* Owned = FindSkill(Option.SkillClass);
	if(!Owned)
	{
		return UPSLibrary::GetOptionBaseDescription(Option);
	}
	
    const int32 NextIndex = Owned->SkillLevel;  // LevelUp이 SkillLevel을 다음 인덱스로 유지
	
    const FSkillLevelData& Delta = Owned->SkillData->LevelData[NextIndex];	// 스킬이 만렙일 때 이 함수가 불리면 터진다. 의도된 것임
	
	TArray<FString> Lines;

	auto AddFlat = [&](float Value, const TCHAR* Label, const TCHAR* Unit)
	{
		if (FMath::IsNearlyZero(Value)) return;
		const TCHAR* Verb = (Value > 0.f) ? TEXT("증가") : TEXT("감소");
		Lines.Add(FString::Printf(TEXT("기본 %s %g%s %s합니다."),
			Label, FMath::Abs(Value), Unit, Verb));
	};

	auto AddPercent = [&](float Value, const TCHAR* Label)
	{
		if (FMath::IsNearlyZero(Value)) return;
		const TCHAR* Verb = (Value > 0.f) ? TEXT("증가") : TEXT("감소");
		Lines.Add(FString::Printf(TEXT("기본 %s %g%% %s합니다."),
			Label, FMath::Abs(Value) * 100.f, Verb));
	};

	auto AddCount = [&](int32 Value, const TCHAR* Label, const TCHAR* Unit)
	{
		if (Value == 0) return;
		const TCHAR* Verb = (Value > 0) ? TEXT("증가") : TEXT("감소");
		Lines.Add(FString::Printf(TEXT("%s %d%s %s합니다."),
			Label, FMath::Abs(Value), Unit, Verb));
	};
	
	AddPercent(Delta.Cooldown, TEXT("쿨타임이"));
	AddFlat(Delta.FireInterval, TEXT("연사 간격이"), TEXT("초"));
	AddCount(Delta.Damage, TEXT("피해가"), TEXT(""));
	
	AddPercent(Delta.AreaMultiplier, TEXT("범위가"));
	
	AddPercent(Delta.ProjectileSpeedMultiplier, TEXT("투사체 속도가"));
	AddCount(Delta.ProjectileCount, TEXT("투사체 갯수가"), TEXT("개"));
	AddCount(Delta.PierceCount, TEXT("관통 횟수가"), TEXT("회"));
	
	AddFlat(Delta.Duration, TEXT("지속시간이"), TEXT("초"));
	AddPercent(Delta.Knockback, TEXT("넉백이"));
	
	AddPercent(Delta.CritChance, TEXT("치명타 확률이"));
	AddPercent(Delta.CritMultiplier, TEXT("치명타 배율이"));
	
	checkf(Lines.Num() > 0,
	TEXT("GetOptionNextLevelDescription: LevelData[%d]의 모든 증감량이 0입니다. 스킬=%s, 데이터에셋 입력 확인"),
	NextIndex, *Owned->SkillData->SkillName);
	
	return FText::FromString(FString::Join(Lines, TEXT("\n")));
}