// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SpawnCasing.h"
#include "FPSCharacter.h"
#include "ViewModelComponent.h"

void UAnimNotify_SpawnCasing::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	if (!MeshComp) {
		return;
	}

	if (AFPSCharacter* Character = Cast<AFPSCharacter>(MeshComp->GetOwner())) {
		Character->GetViewModelComponent()->HandleWeaponCasingEvent();
	}
}