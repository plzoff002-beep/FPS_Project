#include "ExaminationComponent.h"
#include "ExamineStudio.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/PlayerController.h"
#include "ItemTypes.h"
#include "LocalUIManager.h"
#include "UITypes.h"

UExaminationComponent::UExaminationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UExaminationComponent::AdjustSceneCamera(const float Direction) {
	if (!CurrentStudio) return;

	CurrentStudio->AdjustSceneCamera(Direction);
}

void UExaminationComponent::StopExamination() {
	if (CurrentStudio) {
		CurrentStudio->SetActive(false);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController())) {
		PC->SetViewTargetWithBlend(PreviousViewTarget, 0.5, EViewTargetBlendFunction::VTBlend_Cubic);

		PC->ResetIgnoreLookInput();
		PC->ResetIgnoreMoveInput();
	}

	if (UIManager) {
		UIManager->FindAndPopWidget(EUIType::Examination);
	}
}

void UExaminationComponent::StartExamination(FExamineItem* Data) {
	if (!CurrentStudio || !Data) return;

	CurrentStudio->SetExamineData(Data);

	if (APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController())) {
		PreviousViewTarget = PC->GetViewTarget();

		PC->SetViewTargetWithBlend(CurrentStudio, 0.5f, EViewTargetBlendFunction::VTBlend_EaseInOut, 2.f);

		PC->SetIgnoreLookInput(true);
		PC->SetIgnoreMoveInput(true);

		CurrentStudio->SetActive(true);
		CurrentStudio->SetupItem(Data->ExamineActorClass);

		if (UIManager) {
			UIManager->FindAndPushWidget(EUIType::Examination, CurrentStudio);
		}
	}
}

void UExaminationComponent::SetupStudioOnLoaded() {
	if (!StudioClass.IsValid()) return;

	if (UClass* ClassToSpawn = StudioClass.Get()) {
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SpawnLocation = FVector(50000.f, 50000.f, 50000.f);

		CurrentStudio = GetWorld()->SpawnActor<AExamineStudio>(ClassToSpawn, SpawnLocation, FRotator::ZeroRotator, Params);
	}
}

void UExaminationComponent::LoadStudioClassAsync() {
	if (StudioClass.IsNull()) return;

	if (StudioClass.IsValid()) {
		SetupStudioOnLoaded();

		return;
	}

	TWeakObjectPtr<UExaminationComponent> WeakThis(this);

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		StudioClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda([WeakThis]() {
			if (WeakThis.IsValid()) {
				WeakThis->SetupStudioOnLoaded();
			}
		})
	);
}

void UExaminationComponent::SetupStudio() {
	LoadStudioClassAsync();
}

void UExaminationComponent::RotateItem(const FVector2D& Delta) {
	if (CurrentStudio) {
		CurrentStudio->RotateItem(Delta);
	}
}

void UExaminationComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!StudioClass.IsNull()) {
		LoadStudioClassAsync();
	}
	
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController())) {
		if (ULocalPlayer* LP = PC->GetLocalPlayer()) {
			UIManager = LP->GetSubsystem<ULocalUIManager>();
		}
	}
}

