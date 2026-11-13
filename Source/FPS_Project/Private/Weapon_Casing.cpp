#include "Weapon_Casing.h"
#include "GameFramework/ProjectileMovementComponent.h"

AWeapon_Casing::AWeapon_Casing()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	CasingMesh->SetSimulatePhysics(false);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("UProjectileMovementComponent");
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->bRotationFollowsVelocity = true;

	RootComponent = CasingMesh;
}

void AWeapon_Casing::DisableCasing() {
	if (ProjectileMovement) {
		ProjectileMovement->Deactivate();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Weapon_Casing: DisableCasing - ProjectileMovement is nullptr"));
	}

	if (CasingMesh) {
		CasingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CasingMesh->SetVisibility(false);
		CasingMesh->WakeRigidBody();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Weapon_Casing: DisableCasing - CasingMesh is nullptr"))
	}

	SetActorTickEnabled(false);

	bActive = false;
	CurrentLifeTime = 0.f;
}

void AWeapon_Casing::ActivateCasing() {
	if (CasingMesh) {
		CasingMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		CasingMesh->SetCollisionResponseToAllChannels(ECR_Block);
		CasingMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
		CasingMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		CasingMesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
		
		CasingMesh->SetVisibility(true);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Weapon_Casing: ActivateCasing - CasingMesh is nullptr"));
		return;
	}

	if (ProjectileMovement) {
		ProjectileMovement->SetUpdatedComponent(CasingMesh);
		ProjectileMovement->Activate();
		ProjectileMovement->Velocity = GetActorForwardVector() * SpeedValue;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Weapon_Casing: ActivateCasing - ProjectileMovement is nullptr"));
	}

	bActive = true;
	SetActorTickEnabled(true);
}

void AWeapon_Casing::Initialize() {
	if (ProjectileMovement) {
		ProjectileMovement->Bounciness = BounceValue;
		ProjectileMovement->ProjectileGravityScale = GravityValue;
		ProjectileMovement->InitialSpeed = SpeedValue;
		ProjectileMovement->MaxSpeed = SpeedValue * GravityValue;
		
		return;
	} 

	UE_LOG(LogTemp, Error, TEXT("Weapon_Casing: Initialize - ProjectileMovement is nullptr"));
}

void AWeapon_Casing::BeginPlay()
{
	Super::BeginPlay();

	Initialize();
	DisableCasing();
}

void AWeapon_Casing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentLifeTime += DeltaTime;

	if (CurrentLifeTime >= MaxLifeTime) {
		UE_LOG(LogTemp, Warning, TEXT("Despawned"));

		DisableCasing();
	}
}

