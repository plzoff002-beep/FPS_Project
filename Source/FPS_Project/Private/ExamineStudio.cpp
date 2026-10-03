#include "ExamineStudio.h"
#include "Camera/CameraComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "ItemTypes.h"

AExamineStudio::AExamineStudio()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SceneCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("SceneCamera"));
	ItemAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("ItemAnchor"));

	SceneCamera->SetupAttachment(RootComponent);
	ItemAnchor->SetupAttachment(RootComponent);
}


void AExamineStudio::ResetItemAnchor() {
	if (!ItemAnchor) return;

	ItemAnchor->SetWorldRotation(FQuat::Identity);
}

void AExamineStudio::ResetSceneCamera() {
	if (!SceneCamera || !CurrentData) return;

	const float NX = -CurrentData->InitialCameraDistance;

	SceneCamera->SetRelativeLocation(FVector(NX, 0.f, 0.f));
}

void AExamineStudio::AdjustCameraDistanceLimits() {
	if (!SceneCamera || !CurrentData) return;

	const float MaxDistance = -CurrentData->MaxCameraDistance;
	const float MinDistance = -CurrentData->MinCameraDistance;

	if (SceneCamera->GetRelativeLocation().X < MaxDistance) {
		FVector NewDistance = FVector(MaxDistance, 0.f, 0.f);
		SceneCamera->SetRelativeLocation(NewDistance);
	}
	else if (SceneCamera->GetRelativeLocation().X > MinDistance) {
		FVector NewDistance = FVector(MinDistance, 0.f, 0.f);
		SceneCamera->SetRelativeLocation(NewDistance);
	}
}

void AExamineStudio::AdjustSceneCamera(const float Value) {
	if (!SceneCamera || !CurrentData) return;

	const FVector NewLocation = SceneCamera->GetRelativeLocation() + FVector(AdjustingRate * Value, 0.f, 0.f);

	SceneCamera->SetRelativeLocation(NewLocation);

	AdjustCameraDistanceLimits();
}

void AExamineStudio::SetupItemOnLoaded(TSubclassOf<AActor> ClassToSetup) {
	if (!ClassToSetup || ClassToSetup != CurrentClassToLoad.Get() || !bActive) return;

	if (CurrentExamineActor) {
		CurrentExamineActor->Destroy();
	}

	if (UClass* ClassToLoad = ClassToSetup.Get()) {
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentExamineActor = GetWorld()->SpawnActor<AActor>(ClassToLoad, GetActorTransform(), Params);
	}

	if (CurrentExamineActor) {
		CurrentExamineActor->AttachToComponent(ItemAnchor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		
		ResetSceneCamera();
		ResetItemAnchor();
	}
}

void AExamineStudio::LoadItemClassAsync(TSoftClassPtr<AActor> ClassToLoad) {
	if (ClassToLoad.IsNull()) return;

	CurrentClassToLoad = ClassToLoad;

	if (ClassToLoad.IsValid()) {
		SetupItemOnLoaded(ClassToLoad.Get());
		return;
	}

	TWeakObjectPtr<AExamineStudio> WeakThis(this);

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		ClassToLoad.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda([WeakThis, ClassToLoad]() {
			if (WeakThis.IsValid()) {
				UClass* LoadedClass = ClassToLoad.Get();

				WeakThis->SetupItemOnLoaded(LoadedClass);
			}
		})
	);
}

void AExamineStudio::SetupItem(TSoftClassPtr<AActor> ItemToExamine) {
	if (ItemToExamine.IsNull()) return;

	LoadItemClassAsync(ItemToExamine);
}

void AExamineStudio::RotateItem(const FVector2D& Delta) {
	if (!ItemAnchor) return;

	const FVector Up = ItemAnchor->GetUpVector();
	const FVector Right = ItemAnchor->GetRightVector();

	const FQuat YawQ = FQuat(Up, FMath::DegreesToRadians(-Delta.X * SensitivityRotation));
	const FQuat PitchQ = FQuat(Right, FMath::DegreesToRadians(Delta.Y * SensitivityRotation));

	ItemAnchor->AddWorldRotation(YawQ * PitchQ);
}

