// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EquipFinished.h"
#include "FPSCharacter.h"
#include "WeaponTypes.h"
#include "ViewModelComponent.h"

void UAnimNotify_EquipFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	Super::Notify(MeshComp, Animation);

	if (!MeshComp) return;

	AFPSCharacter* Character = Cast<AFPSCharacter>(MeshComp->GetOwner());
	if (!Character) return;

	if (UViewModelComponent* ViewModel = Character->GetViewModelComponent()) {
		ViewModel->HandleMontageFinished(EWeaponAnimStage::EquipFinished);
	}
}