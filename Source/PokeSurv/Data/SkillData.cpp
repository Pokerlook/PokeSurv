// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Data/SkillData.h"

#if WITH_EDITOR
void USkillData::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (LevelData.Num() > 8)
	{
		LevelData.SetNum(8); // 8개 초과시 자동으로 잘라냄
	}
}
#endif
