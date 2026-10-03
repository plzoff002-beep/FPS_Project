// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachmentData.h"

FWeaponStatModifiers UAttachmentData::GetModifiers() const {
	if (bAdjustable) {
		if (CurrentState == EAttachmentState::Folded) {
			return AdjustableModifiers;
		} 
		else {
			return BasicModifiers;
		}
	}

	return BasicModifiers;
}

void UAttachmentData::SetAttachmentState(EAttachmentState NewState) {
	CurrentState = NewState;
}

bool UAttachmentData::IsAttachmentExtended() const {
	return CurrentState == EAttachmentState::Extended;
}