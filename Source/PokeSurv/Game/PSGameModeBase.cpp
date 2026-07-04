// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Game/PSGameModeBase.h"

#include "PSPoolSubsystem.h"
#include "PSWaveSubsystem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "PokeSurv/Combat/PSSkillComponent.h"
#include "PokeSurv/Combat/Item/PSItemBase.h"
#include "PokeSurv/Combat/Pickup/PSPickupBase.h"
#include "PokeSurv/Combat/Skill/PSSkillBase.h"
#include "PokeSurv/Player/PSPlayerStatComponent.h"
#include "PokeSurv/UI/PSHUD.h"
#include "PokeSurv/UI/PSLevelUpMenuWidget.h"
#include "PokeSurv/Util/PSLibrary.h"

void APSGameModeBase::SpawnExpGem(FVector Location, float XPValue)
{
	UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	APSPickupBase* Gem = Cast<APSPickupBase>(
		Pool->SpawnFromPool(ExpGemClass, FVector(Location.X, Location.Y, 0), FRotator::ZeroRotator)
	);

	if (Gem)
	{
		float Total = XPValue + PendingXP;
		PendingXP = 0.f;
		Gem->SetValue(Total);
	}
	else
	{
		PendingXP += XPValue;
	}
}

void APSGameModeBase::AddKillCount()
{
	KillCount++;
	OnKillCountChanged.Broadcast(KillCount);
}

void APSGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (Character)
	{
		PlayerStatComponent = Character->FindComponentByClass<UPSPlayerStatComponent>();
		if (PlayerStatComponent)
		{
			PlayerStatComponent->OnLevelUp.AddDynamic(this, &APSGameModeBase::OnPlayerLevelUp);
		}
		SkillComponent = Character->FindComponentByClass<UPSSkillComponent>();
	}	// 여기서 안 되면 PostLogin?
	
	UPSPoolSubsystem* Pool = GetWorld()->GetSubsystem<UPSPoolSubsystem>();
	if (Pool)
	{
		Pool->InitPool(TSubclassOf<APSPoolableActor>(ExpGemClass), 400);
	}
	
	if (UPSWaveSubsystem* WaveSubsystem = GetWorld()->GetSubsystem<UPSWaveSubsystem>())
	{
		WaveSubsystem->StartWave(WaveTable, EnemyClass, SwarmClass);
	}
}

void APSGameModeBase::OnPlayerLevelUp(int32 NewLevel)
{
	PendingLevelUps++;
    
	if (!bMenuOpen)
	{
		LevelUpSequence();
	}
}

void APSGameModeBase::HandleCardSelected(FPSLevelUpOption SelectedEntry)
{
	if(SelectedEntry.bIsSkill)
	{
		SkillComponent->AddSkill(SelectedEntry.SkillClass);	
	}
	else
	{
		SkillComponent->AddItem(SelectedEntry.ItemClass);		
	}
	
	PendingLevelUps--;

	if (PendingLevelUps > 0)
	{
		LevelUpSequence();   // 메뉴 다시 띄움 (bMenuOpen 유지)
	}
	else
	{
		if (APSHUD* HUD = GetPSHUD())
		{
			HUD->HideLevelUpMenu();
		}
		UGameplayStatics::SetGamePaused(this, false);
		bMenuOpen = false;
	}
}

void APSGameModeBase::LevelUpSequence()
{
	bMenuOpen = true;
    
	// 게임 일시정지
	UGameplayStatics::SetGamePaused(this, true);

	// 후보 풀 생성
	TArray<FPSLevelUpOption> Pool  = BuildCandidatePool();
	
	// 후보 3~4개 추출 SelectCandidates
    TArray<FPSLevelUpOption> Chosen = SelectCandidates(Pool);
	
	
	// 위젯 띄움
	if (APSHUD* HUD = GetPSHUD())
	{
		if (!bMenuBound)
		{
			UPSLevelUpMenuWidget* Menu = HUD->GetOrCreateLevelUpMenu();	// null 가드 안 하는 건 내 의도
			Menu->OnCardSelected.AddDynamic(this, &APSGameModeBase::HandleCardSelected);
			bMenuBound = true;   // 중복 바인딩 방지 플래그
		}
		
		HUD->ShowLevelUpMenu(Chosen);
	}
}

TArray<FPSLevelUpOption> APSGameModeBase::BuildCandidatePool()
{
	TArray<FPSLevelUpOption> CandidatePool;
	
	// 스킬 순회
	for (TSubclassOf<UPSSkillBase> SkillClass : AllSkillClasses)
	{
		if (!SkillClass) continue;

		UPSSkillBase* Owned = SkillComponent->FindSkill(SkillClass);
		if (Owned)
		{
			// 보유 → 만렙 아니면 레벨업 후보
			if (Owned->IsMaxLevel()) continue;
		}
		else
		{
			// 미보유 → 슬롯 여유 있어야 신규 후보
			if (SkillComponent->Skills.Num() >= SkillComponent->MaxSlotPerType) continue;
		}

		// 여기 도달하면 eligible → entry 만들어 Add
		FPSLevelUpOption Entry;
		Entry.bIsSkill = true;
		Entry.SkillClass = SkillClass;
		
		CandidatePool.Add(Entry);
	}
	
	// 아이템 순회
	for (TSubclassOf<UPSItemBase> ItemClass : AllItemClasses)
	{
		if (!ItemClass) continue;

		UPSItemBase* Owned = SkillComponent->FindItem(ItemClass);
		if (Owned)
		{
			// 보유 → 만렙 아니면 레벨업 후보
			if (Owned->IsMaxLevel()) continue;
		}
		else
		{
			// 미보유 → 슬롯 여유 있어야 신규 후보
			if (SkillComponent->Items.Num() >= SkillComponent->MaxSlotPerType) continue;
		}

		FPSLevelUpOption Entry;
		Entry.bIsSkill = false;
		Entry.ItemClass = ItemClass;

		CandidatePool.Add(Entry);
	}
	
	return CandidatePool;
}

TArray<FPSLevelUpOption> APSGameModeBase::SelectCandidates(TArray<FPSLevelUpOption>& Pool)
{
	TArray<FPSLevelUpOption> Result;

	const int32 TargetCount = RollFourthOption() ? 4 : 3;

	// 보유 우선 제안 (인벤토리 안 꽉 찼을 때만, 최대 2회)
	const bool bSkillFull = SkillComponent->Skills.Num() >= SkillComponent->MaxSlotPerType;
	const bool bItemFull  = SkillComponent->Items.Num() >= SkillComponent->MaxSlotPerType;

	if (!(bSkillFull && bItemFull))
	{
		for (int32 i = 0; i < 2; ++i)
		{
			if (!RollOfferOwned()) continue;

			const int32 PickedIndex = PickOwnedIndex(Pool);  // 없으면 INDEX_NONE
			if (PickedIndex == INDEX_NONE) continue;

			Result.Add(Pool[PickedIndex]);
			Pool.RemoveAtSwap(PickedIndex);
		}
	}

	// 남은 슬롯을 가중 추출로 채움 (풀 바닥나면 적게 반환됨 — 정상)
	while (Result.Num() < TargetCount && Pool.Num() > 0)
	{
		const int32 PickedIndex = PickRandomIndex(Pool);
		Result.Add(Pool[PickedIndex]);
		Pool.RemoveAtSwap(PickedIndex);
	}
	
	return Result;
}

int32 APSGameModeBase::PickOwnedIndex(TArray<FPSLevelUpOption>& Pool)
{
	// 새로운 TArray<FPSCandidateEntry> OwnedPool 선언.
    TArray<FPSLevelUpOption> OwnedPool;
	
	// Pool에서 플레이어가 보유중인 아이템,스킬만 OwnedPool에 넣음.
	for (const FPSLevelUpOption& E : Pool)
	{
		const bool bOwned = E.bIsSkill
			? (SkillComponent->FindSkill(E.SkillClass) != nullptr)
			: (SkillComponent->FindItem(E.ItemClass) != nullptr);
		if (bOwned) OwnedPool.Add(E);
	}
	
	if (OwnedPool.Num() == 0) return INDEX_NONE;
	
	// PickRandomIndex에 OwnedPool 넣고 인덱스 받기.
	const int32 OwnedIndex = FMath::RandRange(0, OwnedPool.Num()-1);
	
	// OwnedPool[OwnedIndex]가 원본 Pool에서 몇 번째인지 역추적
    const FPSLevelUpOption& Picked = OwnedPool[OwnedIndex];
	return Pool.IndexOfByPredicate([&](const FPSLevelUpOption& E)
	{
		return E.bIsSkill == Picked.bIsSkill
			&& E.SkillClass == Picked.SkillClass
			&& E.ItemClass == Picked.ItemClass;
	});
}

int32 APSGameModeBase::PickRandomIndex(TArray<FPSLevelUpOption>& Pool)
{
	int Total = 0;
	for (const FPSLevelUpOption& E : Pool)
		Total += GetEntryWeight(E);

	int Roll = FMath::RandRange(0, Total-1);
	for (int32 i = 0; i < Pool.Num(); ++i)
	{
		Roll -= GetEntryWeight(Pool[i]);
		if (Roll < 0)
		{
			return i;
		}
	}
	
	// 반드시 위 for문에서 제대로 리턴이 되어야한다. 안 되면 확인할 수 있게 터뜨려버리자.
	checkf(false, TEXT("PickRandomIndex: 가중치 추출 실패 (Total=%d, Pool=%d)"), Total, Pool.Num());
	return INDEX_NONE;	
}

float APSGameModeBase::GetTotalLuck() const
{
	if (!PlayerStatComponent) return 1.0f;
	return PlayerStatComponent->GetStat(EPlayerStatType::Luck) + 1.0f;
}

bool APSGameModeBase::RollFourthOption() const
{
	const float TotalLuck = GetTotalLuck();
	const float Chance = 1.0f - (1.0f / TotalLuck);
	return FMath::FRand() < Chance;
}

bool APSGameModeBase::RollOfferOwned() const
{
	if (!PlayerStatComponent) return false;
	const float TotalLuck = GetTotalLuck();
	const int32 Level = PlayerStatComponent->PlayerLevel - 1; // 이게 호출 되는 시점에, 호출되기 전 레벨의 +1 되어있음
	const float x = (Level % 2 == 0) ? 2.0f : 1.0f;
	const float Chance = 1.0f + 0.3f * x - (1.0f / TotalLuck);
	return FMath::FRand() < Chance;
}

int32 APSGameModeBase::GetEntryWeight(const FPSLevelUpOption& Entry) const
{
	return Entry.bIsSkill
		? UPSLibrary::GetSkillRarity(Entry.SkillClass)
		: UPSLibrary::GetItemRarity(Entry.ItemClass);
}

APSHUD* APSGameModeBase::GetPSHUD() const
{
	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		return Cast<APSHUD>(PC->GetHUD());
	}
	return nullptr;
}
