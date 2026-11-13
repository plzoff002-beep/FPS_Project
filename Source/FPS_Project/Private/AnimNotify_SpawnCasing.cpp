#include "AnimNotify_SpawnCasing.h"
#include "FPSCharacter.h"
#include "ViewModelComponent.h"

void UAnimNotify_SpawnCasing::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) {
	if (!MeshComp) return;

	AFPSCharacter* Character = Cast<AFPSCharacter>(MeshComp->GetOwner());
	if (!Character) return;

	if (UViewModelComponent* ViewModel = Character->FindComponentByClass<UViewModelComponent>()) {
		ViewModel->HandleWeaponCasingEvent();
	}
}