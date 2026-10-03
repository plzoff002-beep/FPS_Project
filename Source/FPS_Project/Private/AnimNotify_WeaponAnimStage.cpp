// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_WeaponAnimStage.h"
#include "ViewModelComponent.h"
#include "FPSCharacter.h"

void UAnimNotify_WeaponAnimStage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	Super::Notify(MeshComp, Animation);

	if (!MeshComp) {
		return;
	}

	AFPSCharacter* Character = Cast<AFPSCharacter>(MeshComp->GetOwner());
	if (!Character) {
		return;
	}

	if (UViewModelComponent* ViewModel = Character->GetViewModelComponent()) {
		ViewModel->HandleMontageFinished(StageType);
	}
}