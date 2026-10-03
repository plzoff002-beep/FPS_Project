// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Mossberg590.h"
#include "DrawDebugHelpers.h"
//#include "Damageable.h"
//#include "Suppressable.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon_Casing.h"
#include "WeaponData.h"

void AWeapon_Mossberg590::BeginPlay() {
	Super::BeginPlay();
}

void AWeapon_Mossberg590::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AWeapon_Mossberg590::Shoot() {
	if (!Data || !CanShoot()) return;

	if (!WeaponMesh->DoesSocketExist(TEXT("Muzzle"))) return;

	if (Data->bChargeEveryShoot) {
		bNeedCharge = true;
	}

	PlayEventSound(EWeaponAnimEvent::Shot);

	Ammo -= 1;

	FireCooldown = Data->FireRate;

	OnWeaponAction.Broadcast(Data, EWeaponMontage::Shoot);
	OnAmmoChanged.Broadcast(Ammo, TotalAmmo);

	FVector Origin = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));

	for (int32 Shot = 0; Shot < Data->ShotsAmount; Shot++) {
		FVector Direction = WeaponMesh->GetSocketRotation(TEXT("Muzzle")).Vector();

		Direction.Y += FMath::FRandRange(-Data->Spread, Data->Spread);
		Direction.Z += FMath::FRandRange(-Data->Spread, Data->Spread);
		Direction.Normalize();

		FVector End = Origin + Direction * Data->Range;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		FHitResult Hit;

		//HandleSuppress(Origin, Direction);

		if (GetWorld()->LineTraceSingleByChannel(Hit, Origin, End, ECC_Visibility, Params)) {
			DrawDebugSphere(GetWorld(), Hit.Location, 10.f, 8, FColor::Red, false, 2.f);

			//HandleHit(Hit);
		}

		DrawDebugLine(GetWorld(), Origin, End, FColor::Red, false, 2.f, 0, 2.f);
	}
}