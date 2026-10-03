// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"
#include "Components/Button.h"

void UMenuWidget::UnifiedButtonHandler(EButtonType ButtonType, UObject*) {
	UE_LOG(LogTemp, Warning, TEXT("Start"));

	if (OnButtonClicked.IsBound()) {
		UE_LOG(LogTemp, Warning, TEXT("Proccess"));
		OnButtonClicked.Broadcast(ButtonType, this);
	}
}

void UMenuWidget::NativeConstruct() {
	PlayButton->BindDelegate();
	OptionsButton->BindDelegate();
	QuitButton->BindDelegate();

	PlayButton->OnButtonClicked.AddUObject(
		this,
		&UMenuWidget::UnifiedButtonHandler
	);

	OptionsButton->OnButtonClicked.AddUObject(
		this,
		&UMenuWidget::UnifiedButtonHandler
	);

	QuitButton->OnButtonClicked.AddUObject(
		this,
		&UMenuWidget::UnifiedButtonHandler
	);
}