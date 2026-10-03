// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "DrawDebugHelpers.h"
//#include "Damageable.h"
//#include "Suppressable.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon_Casing.h"
#include "WeaponData.h"
#include "WeaponAttachmentComponent.h"
#include "AttachmentData.h"
#include "WeaponIKTypes.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalWeaponMesh"));
	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkeletalMagazineMesh"));

	MagazineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MagazineMesh->SetSimulatePhysics(false);

	RootComponent = WeaponMesh;

	AttachmentComponent = CreateDefaultSubobject<UWeaponAttachmentComponent>(TEXT("WeaponAttachmentComponent"));
}

FWeaponIKData AWeaponBase::GetWeaponIKData() const {
	FWeaponIKData IKData{};

	if (AttachmentComponent) {

		if (AttachmentComponent->HasGripAttachment()) {
			FTransform GripSocketTransform = AttachmentComponent->GetGripSocketTransform();

			if (Owner && Owner->GetClass()->ImplementsInterface(UWeaponIKProvider::StaticClass())) {
				FTransform HandRTransform = IWeaponIKProvider::Execute_GetRightHandIKTransform(Owner);

				FTransform RelativeTransform = GripSocketTransform.GetRelativeTransform(HandRTransform);

				IKData.HasGrip = true;
				IKData.LeftHandTarget = RelativeTransform;
				IKData.IKAlpha = 1.f;
			}
		}
	}

	return IKData;
}

//void AWeaponBase::HandleSuppress(const FVector& Origin, const FVector& Direction) {
//	FVector End = (Origin + Direction * Data->Range);
//
//	TArray<AActor*> Actors;
//
//	UGameplayStatics::GetAllActorsWithInterface(
//		GetWorld(),
//		USuppressable::StaticClass(),
//		Actors
//	);
//
//	for (AActor* Actor : Actors) {
//		FVector NPCLocation = Actor->GetActorLocation();
//		FVector ToNPC = NPCLocation - Origin;
//
//		float ProjectDistance = (Direction.X * ToNPC.X) + (Direction.Y * ToNPC.Y) + (Direction.Z * ToNPC.Z);
//
//		if (ProjectDistance < 0 || ProjectDistance > Data->Range) continue;
//
//		FVector ProjectPoint = Origin + Direction * ProjectDistance;
//
//		float Distance = FVector::Dist(NPCLocation, ProjectPoint);
//
//		if (Distance <= MaxSuppressDistance) {
//			if (Actor && Actor->GetClass()->ImplementsInterface(USuppressable::StaticClass())) {
//				const float MaxSuppress = Data->SuppressPenalty;
//
//				float SuppressValue = MaxSuppress * (1.f - Distance / MaxSuppressDistance);
//
//				ISuppressable::Execute_ApplySuppression(Actor, SuppressValue);
//			}
//		}
//	}
//}
//
//void AWeaponBase::HandleHit(const FHitResult& Hit) {
//	AActor* HitActor = Hit.GetActor();
//
//	if (HitActor && HitActor->GetClass()->ImplementsInterface(UDamageable::StaticClass())) {
//		IDamageable::Execute_ApplyDamage(HitActor, Data->Damage);
//	}
//}

//void AWeaponBase::TryToggleLaserAttachments() {
//	if (SpawnedAttachmentActors.IsEmpty()) return;
//
//	for (const auto& [ActorKey, ActorValue] : SpawnedAttachmentActors) {
//		if (ActorKey != EAttachmentType::Laser) {
//			continue;
//		}
//
//		if (ActorValue->Implements<UAttachmentInterface>()) {
//			IAttachmentInterface::Execute_ToggleAttachment(ActorValue);
//		}
//	}
//}

void AWeaponBase::HandleChangeSpread() {
	if (!AttachmentComponent || AttachmentComponent->CurrentAttachments.IsEmpty() || !Data) return;

	WeaponStats.Spread = 0.f;

	for (const auto& It : AttachmentComponent->CurrentAttachments) {
		WeaponStats.Spread += Data->Spread * It.Value.Data->GetModifiers().SpreadMultiplier;
	}
}

void AWeaponBase::SetIsNeedCharge(bool bValue) {
	bNeedCharge = bValue;
}

void AWeaponBase::AddAmmo(int32 Value) {
	TotalAmmo -= Value;
	Ammo += Value;

	OnAmmoChanged.Broadcast(Ammo, TotalAmmo);
}

void AWeaponBase::PlayEventSound(EWeaponAnimEvent EventType) const {
	if (const TObjectPtr<USoundBase>* FoundSound = Data->AnimEventSounds.Find(EventType)) {
		UGameplayStatics::PlaySoundAtLocation(this, *FoundSound, GetActorLocation());
	}
}

void AWeaponBase::SpawnCasing() const {
	FActorSpawnParameters Params;
	Params.Owner = GetOwner();

	const FTransform SocketTransform = WeaponMesh->GetSocketTransform(Data->SocketCasing);

	AWeapon_Casing* Casing = GetWorld()->SpawnActor<AWeapon_Casing>(
		Data->CasingMesh, 
		SocketTransform,
		Params
	);
}

void AWeaponBase::AddModifiersStats() {
	if (!AttachmentComponent) return;

	FWeaponStatModifiers Stats = AttachmentComponent->GetTotalModifiers();

	WeaponStats.Damage += Stats.AdditiveDamage;
	WeaponStats.Recoil *= Stats.RecoilMultiplier;
	WeaponStats.Spread *= Stats.SpreadMultiplier;
	WeaponStats.AdsSpread *= Stats.AdsSpreadMultiplier;
}

void AWeaponBase::InitStats() {
	Ammo = Data->ClipSize;
	TotalAmmo = Data->TotalAmmo;

	WeaponStats.Damage = Data->Damage;
	WeaponStats.FireRate = Data->FireRate;
	WeaponStats.Recoil = Data->Recoil;
	WeaponStats.Spread = Data->Spread;
	WeaponStats.AdsSpread = Data->AdsSpread;
}

void AWeaponBase::Initialize() {
	if (!Data) return;

	InitStats();

	AddModifiersStats();

	WeaponMesh->SetSkeletalMesh(Data->WeaponMesh);
	WeaponMesh->SetAnimInstanceClass(Data->WeaponAnimBP);

	MagazineMesh->SetStaticMesh(Data->MagazineMesh);
	MagazineMesh->AttachToComponent(WeaponMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, Data->SocketMagazine);
}

void AWeaponBase::Reload() {
	if (!Data) return;

	if (TotalAmmo <= 0 || Ammo == Data->ClipSize) return;

	if (!bNeedCharge) {
		bNeedCharge = Ammo <= 0 ? true : false;
	}

	if (Data->ReloadType == EWeaponReloadType::Standart) {
		OnWeaponAction.Broadcast(Data, EWeaponMontage::Reload);
	}
	else {
		OnWeaponAction.Broadcast(Data, EWeaponMontage::Reload_Start);
	}
}

void AWeaponBase::ReloadFinished() {
	if (!Data) return;

	int32 NeedAmmo = Data->ClipSize - Ammo;
	int32 TakeAmmo = FMath::Min(TotalAmmo, NeedAmmo);

	TotalAmmo -= TakeAmmo;
	Ammo += TakeAmmo;

	OnAmmoChanged.Broadcast(Ammo, TotalAmmo);
}

bool AWeaponBase::IsChargeEveryShoot() const {
	return Data->bChargeEveryShoot;
}

bool AWeaponBase::IsClipFull() const {
	return Ammo >= Data->ClipSize;
}

bool AWeaponBase::CanShoot() const {
	if (Ammo <= 0 || FireCooldown > 0.f || bNeedCharge) {
		return false;
	}

	return true;
}

void AWeaponBase::Shoot() {
	if (!Data || !CanShoot()) return;

	if (!WeaponMesh->DoesSocketExist(TEXT("Muzzle"))) return;
	
	if (Data->bChargeEveryShoot) {
		bNeedCharge = true;
	}

	PlayEventSound(EWeaponAnimEvent::Shot);

	Ammo -= 1;

	FireCooldown = WeaponStats.FireRate;

	OnWeaponAction.Broadcast(Data, EWeaponMontage::Shoot);
	OnAmmoChanged.Broadcast(Ammo, TotalAmmo);

	FVector Origin = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));
	FVector Direction = WeaponMesh->GetSocketRotation(TEXT("Muzzle")).Vector();
	
	Direction.Y += FMath::FRandRange(-WeaponStats.Spread, WeaponStats.Spread);
	Direction.Z += FMath::FRandRange(-WeaponStats.Spread, WeaponStats.Spread);
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

void AWeaponBase::SetActorHiddenInGame(bool bNewHidden) {
	Super::SetActorHiddenInGame(bNewHidden);

	AttachmentComponent->SetAttachmentsHiddenInGame(bNewHidden);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	AttachmentComponent->Initialize();

	Initialize();
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FireCooldown > 0.f) {
		FireCooldown -= DeltaTime;
	}
}

