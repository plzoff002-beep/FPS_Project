// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserAttachment.h"
#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ALaserAttachment::ALaserAttachment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LaserMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserMesh"));
	LaserMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	RootComponent = LaserMesh;

	LaserBeamComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserBeamComponent"));
	LaserBeamComponent->SetupAttachment(RootComponent);
}

void ALaserAttachment::ToggleAttachment_Implementation() {
	if (bIsLaserActive) {
		LaserBeamComponent->Deactivate();
		bIsLaserActive = false;
	}
	else {
		LaserBeamComponent->Activate();
		bIsLaserActive = true;
	}
}

void ALaserAttachment::SetActorHiddenInGame(bool bNewHidden) {
	Super::SetActorHiddenInGame(bNewHidden);

	if (LaserBeamComponent) {
		if (bNewHidden) {
			LaserBeamComponent->Deactivate();
		}
		else {
			if (bIsLaserActive) {
				LaserBeamComponent->Activate();
			}
		}
	}
}

// Called every frame
void ALaserAttachment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!LaserBeamComponent) return;

	FRotator SocketRotation = LaserMesh->GetSocketRotation(LaserSocket);

	FVector Origin = LaserMesh->GetSocketLocation(LaserSocket);
	FVector Direction = SocketRotation.Vector();
	FVector End = Origin + Direction * MaxDist;

	FHitResult Hit;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, Origin, End, ECC_Visibility, Params);

	FVector FinalBeamEnd = bBlocked ? Hit.ImpactPoint : End;

	LaserBeamComponent->SetVariableVec3(FName("User.BeamStart"), Origin);
	LaserBeamComponent->SetVariableVec3(FName("User.BeamEnd"), FinalBeamEnd);
}

