// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon_Casing.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AWeapon_Casing::AWeapon_Casing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	CasingMesh->SetCollisionResponseToAllChannels(ECR_Block);
	CasingMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	CasingMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CasingMesh->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("UProjectileMovementComponent");
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->bRotationFollowsVelocity = true;

	RootComponent = CasingMesh;
}

void AWeapon_Casing::Initialize() {
	ProjectileMovement->UpdatedComponent = CasingMesh;
	ProjectileMovement->Bounciness = BounceValue;
	ProjectileMovement->ProjectileGravityScale = GravityValue;
	ProjectileMovement->InitialSpeed = SpeedValue;
	ProjectileMovement->MaxSpeed = SpeedValue * GravityValue;
	ProjectileMovement->Velocity = GetActorForwardVector() * SpeedValue;
}

// Called when the game starts or when spawned
void AWeapon_Casing::BeginPlay()
{
	Super::BeginPlay();

	Initialize();

	SetLifeSpan(LifeTime);
}

// Called every frame
void AWeapon_Casing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

