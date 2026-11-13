#include "WeaponBase.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon_Casing.h"
#include "WeaponData.h"
#include "WeaponAttachmentComponent.h"
#include "AttachmentData.h"
#include "WeaponIKTypes.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalWeaponMesh"));
	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkeletalMagazineMesh"));

	MagazineMesh.Get()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MagazineMesh.Get()->SetSimulatePhysics(false);

	RootComponent = WeaponMesh.Get();

	AttachmentComponent = CreateDefaultSubobject<UWeaponAttachmentComponent>(TEXT("WeaponAttachmentComponent"));
}

FWeaponIKData AWeaponBase::GetWeaponIKData() const {
	FWeaponIKData IKData{};

	if (AttachmentComponent) {
		if (AttachmentComponent.Get()->HasGripAttachment()) {
			FTransform GripSocketTransform = AttachmentComponent.Get()->GetGripSocketTransform();
			if (AActor* OwnerActor = GetOwner()) {
				if (IWeaponIKProvider* Provider = Cast<IWeaponIKProvider>(OwnerActor)) {
					FTransform HandRTransform = Provider->GetRightHandIKTransform();

					FTransform RelativeTransform = GripSocketTransform.GetRelativeTransform(HandRTransform);

					IKData.HasGrip = true;
					IKData.LeftHandTarget = RelativeTransform;
					IKData.IKAlpha = 1.f;
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("WeaponBase: GetWeaponIKData - Provider is nullptr"));
				}
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("WeaponBase: GetWeaponIKData - Owner is nullptr"), *GetNameSafe(OwnerActor));
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("WeaponBase: GetWeaponIKData - No Grip Attachment"));
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("WeaponBase: GetWeaponIKData - AttachmentComponent is nullptr"));
	}

	return IKData;
}

UStaticMeshComponent* AWeaponBase::GetAttachmentMesh(EAttachmentType TargetType) const {
	if (!AttachmentComponent || AttachmentComponent->IsAttachmentComponentsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("WeaponBase: GetAttachmentMesh. AttachmentComponent: %s, IsListEmpty: %s"),
			*GetNameSafe(AttachmentComponent.Get()),
			(AttachmentComponent && AttachmentComponent.Get()->IsAttachmentComponentsEmpty()) ? TEXT("True") : TEXT("False")
		);

		return nullptr;
	}

	return AttachmentComponent.Get()->GetAttachmentMesh(TargetType);
}

const float AWeaponBase::GetRecoil() const {
	if (Data) {
		return Data.Get()->Recoil;
	}

	UE_LOG(LogTemp, Warning, TEXT("WeaponBase: GetRecoil - Data is nullptr"));
	return 0.f;
}

void AWeaponBase::HandleChangeSpread() {
	if (!AttachmentComponent || AttachmentComponent.Get()->IsCurrentAttachmentsEmpty() || !Data) {
		UE_LOG(LogTemp, Error, TEXT("WeaponBase: HandleChangeSpread. AttachmentComponent: %s, IsEmpty: %s, Data: %s"), 
			*GetNameSafe(AttachmentComponent.Get()),
			(AttachmentComponent && AttachmentComponent->IsCurrentAttachmentsEmpty()) ? TEXT("True") : TEXT("False"),
			*GetNameSafe(Data.Get())
		);

		return;
	}

	WeaponStats.Spread = 0.f;

	for (const auto& It : AttachmentComponent->GetCurrentAttachments()) {
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

void AWeaponBase::HandlePlayEventSound(EWeaponAnimEvent EventType) const {
	if (!Data) {
		UE_LOG(LogTemp, Error, TEXT("WeaponBase: HandlePlayEventSound - Data is nullptr"));
		return;
	};

	if (USoundBase* FoundSound = Data.Get()->AnimEventSounds.FindRef(EventType)) {
		UGameplayStatics::PlaySoundAtLocation(this, FoundSound, GetActorLocation());
	}
}

void AWeaponBase::AddModifiersStats() {
	if (!AttachmentComponent) {
		UE_LOG(LogTemp, Error, TEXT("WeaponBase: AddModifiersStats - AttachmentComponent is nullptr"));
		return;
	}

	FWeaponStatModifiers Stats = AttachmentComponent->GetTotalModifiers();

	WeaponStats.Damage += Stats.AdditiveDamage;
	WeaponStats.Recoil *= Stats.RecoilMultiplier;
	WeaponStats.Spread *= Stats.SpreadMultiplier;
	WeaponStats.AdsSpread *= Stats.AdsSpreadMultiplier;
}

void AWeaponBase::HandleSpawnCasing() const {
	if (CasingPool.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("WeaponBase: HandleSpawnCasing - CasingPool is Empty for weapon '%s'"),
			*Data.Get()->WeaponName.ToString()
		);

		return;
	}

	for (AWeapon_Casing* Casing : CasingPool) {
		if (!Casing->IsActive()) {
			FTransform SocketTransform = WeaponMesh.Get()->GetSocketTransform(Data.Get()->SocketCasing);

			Casing->SetActorTransform(SocketTransform);
			Casing->ActivateCasing();

			return;
		}
	}
}

void AWeaponBase::SpawnCasingPool() {
	if (!Data) {
		UE_LOG(LogTemp, Error, TEXT("WeaponBase: SpawnCasingPool - Data is nullptr"));
		return;
	}

	FActorSpawnParameters Params;
	Params.Owner = GetOwner();

	const int32 CasingMaxAmount = Data.Get()->CasingPoolAmount;

	for (int32 i = 0; i < CasingMaxAmount; ++i) {
		AWeapon_Casing* Casing = GetWorld()->SpawnActor<AWeapon_Casing>(
			Data.Get()->CasingMesh,
			FTransform(),
			Params
		);

		if (Casing) {
			CasingPool.Add(Casing);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("WeaponBase: SpawnCasingPool - Failed spawn casing for weapon: '%s' at Iteration: %i"),
				*Data.Get()->WeaponName.ToString(),
				i
			);
		}
	}
}

void AWeaponBase::InitializeStats() {
	if (!Data) {
		UE_LOG(LogTemp, Error, TEXT("WeaponBase: InitializeStats - Data is nullptr"));
		return;
	}

	UWeaponData* DataRaw = Data.Get();

	Ammo = DataRaw->ClipSize;
	TotalAmmo = DataRaw->TotalAmmo;

	WeaponStats.Damage = DataRaw->Damage;
	WeaponStats.FireRate = DataRaw->FireRate;
	WeaponStats.Recoil = DataRaw->Recoil;
	WeaponStats.Spread = DataRaw->Spread;
	WeaponStats.AdsSpread = DataRaw->AdsSpread;
}

void AWeaponBase::Initialize() {
	if (!Data || !WeaponMesh || !MagazineMesh) {
		UE_LOG(LogTemp, Error, TEXT("WeaponBase: Initialize. Data: %s, WeaponMesh: %s, MagazineMesh: %s"),
			*GetNameSafe(Data.Get()),
			*GetNameSafe(WeaponMesh.Get()),
			*GetNameSafe(MagazineMesh.Get())
		);

		return;
	}

	InitializeStats();
	AddModifiersStats();
	SpawnCasingPool();
	
	WeaponMesh.Get()->SetSkeletalMesh(Data->WeaponMesh.Get());
	WeaponMesh.Get()->SetAnimInstanceClass(Data->WeaponAnimBP);

	MagazineMesh.Get()->SetStaticMesh(Data->MagazineMesh.Get());
	MagazineMesh.Get()->AttachToComponent(WeaponMesh.Get(), FAttachmentTransformRules::SnapToTargetIncludingScale, Data->SocketMagazine);
}

bool AWeaponBase::Reload() {
	if (!Data) {
		UE_LOG(LogTemp, Error, TEXT("WeaponBase: Reload - Data is nullptr"));
		return false;
	};

	if (TotalAmmo <= 0 || Ammo == Data.Get()->ClipSize) return false;

	if (!bNeedCharge) {
		bNeedCharge = Ammo <= 0 ? true : false;
	}

	if (Data.Get()->ReloadType == EWeaponReloadType::Standart) {
		OnWeaponAction.Broadcast(Data.Get(), EWeaponMontage::Reload);
	}
	else {
		OnWeaponAction.Broadcast(Data.Get(), EWeaponMontage::Reload_Start);
	}

	return true;
}

void AWeaponBase::ReloadFinished() {
	if (!Data) {
		UE_LOG(LogTemp, Error, TEXT("WeaponBase: ReloadFinished - Data is nullptr"));
		return;
	};

	int32 NeedAmmo = Data.Get()->ClipSize - Ammo;
	int32 TakeAmmo = FMath::Min(TotalAmmo, NeedAmmo);

	TotalAmmo -= TakeAmmo;
	Ammo += TakeAmmo;

	OnAmmoChanged.Broadcast(Ammo, TotalAmmo);
}

bool AWeaponBase::IsChargeEveryShoot() const {
	return Data.Get()->bChargeEveryShoot;
}

bool AWeaponBase::IsClipFull() const {
	return Ammo >= Data.Get()->ClipSize;
}

bool AWeaponBase::CanShoot() const {
	if (Ammo <= 0 || FireCooldown > 0.f || bNeedCharge) {
		return false;
	}

	return true;
}

bool AWeaponBase::Shoot() {
	if (!Data || !WeaponMesh || !WeaponMesh->DoesSocketExist(TEXT("Muzzle"))) {
		UE_LOG(LogTemp, Error, TEXT("WeaponBase: Shoot. Data: %s, WeaponMesh: %s, SocketExist: %s"),
			*GetNameSafe(Data.Get()),
			*GetNameSafe(WeaponMesh.Get()),
			(WeaponMesh && WeaponMesh.Get()->DoesSocketExist(TEXT("Muzzle"))) ? TEXT("True") : TEXT("False")
		);

		return false;
	}

	if (!CanShoot()) {
		return false;
	}
	
	if (Data.Get()->bChargeEveryShoot) {
		bNeedCharge = true;
	}

	HandlePlayEventSound(EWeaponAnimEvent::Shot);

	Ammo -= 1;

	FireCooldown = WeaponStats.FireRate;
	SetActorTickEnabled(true);

	OnWeaponAction.Broadcast(Data.Get(), EWeaponMontage::Shoot);
	OnAmmoChanged.Broadcast(Ammo, TotalAmmo);

	FVector Origin = WeaponMesh.Get()->GetSocketLocation(TEXT("Muzzle"));
	FVector Direction = WeaponMesh.Get()->GetSocketRotation(TEXT("Muzzle")).Vector();
	
	Direction.Y += FMath::FRandRange(-WeaponStats.Spread, WeaponStats.Spread);
	Direction.Z += FMath::FRandRange(-WeaponStats.Spread, WeaponStats.Spread);
	Direction.Normalize();

	FVector End = Origin + Direction * Data.Get()->Range;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult Hit;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Origin, End, ECC_Visibility, Params)) {
		DrawDebugSphere(GetWorld(), Hit.Location, 10.f, 8, FColor::Red, false, 2.f);
	}

	DrawDebugLine(GetWorld(), Origin, End, FColor::Red, false, 2.f, 0, 2.f);

	return true;
}

void AWeaponBase::SetActorHiddenInGame(bool bNewHidden) {
	Super::SetActorHiddenInGame(bNewHidden);

	AttachmentComponent->SetAttachmentsHiddenInGame(bNewHidden);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	AttachmentComponent->Initialize();

	Initialize();
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireCooldown -= DeltaTime;

	if (FireCooldown <= 0.f) {
		FireCooldown = 0.f;
		SetActorTickEnabled(false);
	}
}

