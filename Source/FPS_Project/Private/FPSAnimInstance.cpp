// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraComponent.h"
#include "FPSAnimInstance.h"
#include "CharacterStateProvider.h"

void UFPSAnimInstance::UpdateLeftHandIK(float DeltaSeconds) {
	if (!Owner) {
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
	if (!Owner) return;

	if (Owner->GetClass()->ImplementsInterface(UWeaponIKProvider::StaticClass())) {
		IKData = IWeaponIKProvider::Execute_GetWeaponIKData(Owner);
	}
	else {
		IKData = {};
	}
}

void UFPSAnimInstance::TryGetViewModelState() {
	if (!Owner) return;

	if (Owner->GetClass()->ImplementsInterface(UCharacterStateProvider::StaticClass())) {
		ViewModelState = ICharacterStateProvider::Execute_GetViewModelState(Owner);
	}
	else {
		ViewModelState = {};
	}
}

void UFPSAnimInstance::TryGetCharacterState() {
	if (!Owner) return;

	if (Owner->GetClass()->ImplementsInterface(UCharacterStateProvider::StaticClass())) {
		CharacterState = ICharacterStateProvider::Execute_GetCharacterState(Owner);
	}
	else {
		CharacterState = {};
	}
}

void UFPSAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds) {
	UpdateLeftHandIK(DeltaSeconds);
}

void UFPSAnimInstance::NativeInitializeAnimation() {
	Owner = Cast<AActor>(TryGetPawnOwner());
}

void UFPSAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	TryGetCharacterState();
	TryGetViewModelState();
	TryGetWeaponIKData();
}