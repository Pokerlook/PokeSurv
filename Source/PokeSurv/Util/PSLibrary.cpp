// Fill out your copyright notice in the Description page of Project Settings.


#include "PokeSurv/Util/PSLibrary.h"

#include "PokeSurv/Interface/Damageable.h"

void UPSLibrary::ApplyDamage(AActor* Target, FDamageResult Damage, float KnockBackMultiplier, AActor* DamageCauser)
{
	if (!Target || !DamageCauser) return;

	if (DamageCauser->ActorHasTag("Player"))
	{
		if (!Target->ActorHasTag("Enemy")) return;
	}
	else if (DamageCauser->ActorHasTag("Enemy"))
	{
		if (!Target->ActorHasTag("Player")) return;
	}
	else return;

	if (Target->Implements<UDamageable>())
	{
		IDamageable::Execute_PS_TakeDamage(Target, Damage);
		if (KnockBackMultiplier > 0.f)
		{
			IDamageable::Execute_TakeKnockback(Target, KnockBackMultiplier, DamageCauser->GetActorLocation());
		}
	}
}
