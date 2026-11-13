// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_WeaponEvent.h"
#include "FPSCharacter.h"
#include "ViewModelComponent.h"

void UAnimNotify_WeaponEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	if (!MeshComp) return;

	AFPSCharacter* Character = Cast<AFPSCharacter>(MeshComp->GetOwner());
	if (!Character) return;

	if (UViewModelComponent* ViewModel = Character->FindComponentByClass<UViewModelComponent>()) {
		ViewModel->HandleWeaponAnimEvent(EventType);
	}
}