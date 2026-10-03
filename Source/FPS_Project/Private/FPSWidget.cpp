#include "FPSWidget.h"
#include "Components/ProgressBar.h"
#include "WeaponManager.h"
#include "StaminaComponent.h"

void UFPSWidget::InitHUD(const float HealthAmount) {
	HandleAmmoChanged(0, 0);

	SetHealth(HealthAmount);

	if (APawn* Owner = GetOwningPlayerPawn()) {
		if (UWeaponManager* WP = Owner->FindComponentByClass<UWeaponManager>()) {
			WP->OnAmmoChanged.AddUObject(
				this,
				&UFPSWidget::HandleAmmoChanged
			);
		}

		if (UStaminaComponent* SC = Owner->FindComponentByClass<UStaminaComponent>()) {
			SC->OnStaminaChanged.AddUObject(
				this,
				&UFPSWidget::HandleStaminaChanged
			);
		}
	}
}

void UFPSWidget::HandleStaminaChanged(const float NewValue) {
	if (StaminaBar) {
		StaminaBar->SetPercent(NewValue);
	}
}

void UFPSWidget::HandleAmmoChanged(const int32 CurrentAmmoValue, const int32 TotalAmmoValue) {
	if (CurrentAmmo) {
		CurrentAmmo->SetText(FText::AsNumber(CurrentAmmoValue));
	}

	if (TotalAmmo) {
		TotalAmmo->SetText(FText::AsNumber(TotalAmmoValue));
	}
}

void UFPSWidget::SetHealth(const float HealthValue) {
	if (Health) {
		Health->SetText(FText::AsNumber(HealthValue));
	}
}

void UFPSWidget::NativeConstruct() {
	Super::NativeConstruct();

	InitHUD(100.f);
}

void UFPSWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);
}