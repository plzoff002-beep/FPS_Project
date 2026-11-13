#include "Camera/CameraComponent.h"
#include "FPSStateComponent.h"
#include "FPSAnimInstance.h"
#include "CharacterStateProvider.h"

void UFPSAnimInstance::SetOwner(AActor* NewOwner) {
	Owner = NewOwner;

	if (!Owner.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("FPSAnimInstance: SetOwner - Owner linking is failed"));
	}
}

void UFPSAnimInstance::SetStateComp(UFPSStateComponent* NewComp) {
	StateComp = NewComp;

	if (!StateComp.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("FPSAnimInstance: SetStateComp - StateComp linking is failed"));
	}
}

void UFPSAnimInstance::UpdateLeftHandIK(float DeltaSeconds) {
	if (!Owner.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("FPSAnimInstance: UpdateLeftHandIK - Owner is nullptr"))
		return;
	}

	const TMap<FName, float>& ActiveCurves = GetAnimationCurveList(EAnimCurveType::AttributeCurve);

	float MontageCurveWeight = 1.f;

	if (ActiveCurves.Contains(FName("LeftHandIKWeight"))) {
		MontageCurveWeight = GetCurveValue(FName("LeftHandIKWeight"));
	}

	float TargetAlpha = IKData.IKAlpha * MontageCurveWeight;

	CurrentIKAlpha = FMath::FInterpTo(CurrentIKAlpha, TargetAlpha, DeltaSeconds, IKAccel);

	if (CurrentIKAlpha > 0.001) {
		FVector NewLoc = FMath::VInterpTo(CurrentLeftHandIKTransform.GetLocation(), 
			IKData.LeftHandTarget.GetLocation(), DeltaSeconds,
			LHandIKAccel
		);


		FQuat NewRot = FQuat::Slerp(CurrentLeftHandIKTransform.GetRotation(),
			IKData.LeftHandTarget.GetRotation(),
			FMath::Clamp(DeltaSeconds * IKAccel, 0.f, 1.f)
		);

		CurrentLeftHandIKTransform.SetLocation(NewLoc);
		CurrentLeftHandIKTransform.SetRotation(NewRot);
	}
}

void UFPSAnimInstance::TryGetWeaponIKData() {
	if (!Owner.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("FPSAnimInstance: TryGetWeaponIKData - Owner is nullptr"))
		return;
	}

	if (IWeaponIKProvider* Provider = Cast<IWeaponIKProvider>(Owner.Get())) {
		IKData = Provider->GetWeaponIKData();
	}
	else {
		IKData = {};
	}
}

void UFPSAnimInstance::TryGetViewModelState() {
	if (!Owner.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("FPSAnimInstance: TryGetViewModelState - Owner is nullptr"))
		return;
	}

	if (ICharacterStateProvider* Provider = Cast<ICharacterStateProvider>(Owner.Get())) {
		ViewModelState = Provider->GetViewModelState();
	}
	else {
		ViewModelState = {};
	}
}

void UFPSAnimInstance::TryGetCharacterState() {
	if (!StateComp.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("FPSAnimInstance: TryGetCharacterState - StateComp is nullptr"))
		return;
	}

	StateList = StateComp.Get()->GetStateList();
}

void UFPSAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds) {
	UpdateLeftHandIK(DeltaSeconds);
}

void UFPSAnimInstance::NativeInitializeAnimation() {
	SetOwner(Cast<AActor>(TryGetPawnOwner()));

	if (Owner.IsValid()) {
		SetStateComp(Owner.Get()->FindComponentByClass<UFPSStateComponent>());
	}
}

void UFPSAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	TryGetCharacterState();
	TryGetViewModelState();
	TryGetWeaponIKData();
}