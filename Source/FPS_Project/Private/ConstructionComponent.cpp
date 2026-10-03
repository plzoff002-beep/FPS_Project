#include "ConstructionComponent.h"
#include "BuildObjectData.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "FPSCollisionChannels.h"

UConstructionComponent::UConstructionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(false);
}

FVector UConstructionComponent::GetPlacementLocation() {
	FVector Loc = PlayerCamera->GetComponentLocation();
	FVector Dir = PlayerCamera->GetForwardVector();
	FVector End = Loc + Dir * MaxBuildDistance;

	FVector SnappedLocation;
	SnappedLocation.X = FMath::GridSnap(End.X, CurrentBuildData->GridSize.X);
	SnappedLocation.Y = FMath::GridSnap(End.Y, CurrentBuildData->GridSize.Y);
	SnappedLocation.Z = FMath::GridSnap(End.Z, CurrentBuildData->GridSize.Z);

	return SnappedLocation;
}

bool UConstructionComponent::IsBlocked(const FVector& Location, UStaticMeshComponent* MeshComp) {
	FComponentQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.AddIgnoredActor(GhostActor);

	TArray<FOverlapResult> Overlaps;

	GetWorld()->ComponentOverlapMultiByChannel(Overlaps, MeshComp, Location, RotateTarget.Quaternion(), CollisionChannels::BuildableObject, Params);

	return Overlaps.Num() > 0;
}

bool UConstructionComponent::IsTooDeep(const FVector& Location, const FHitResult& GroundHit, const float ExtentHeight) {
	const float CurrentSinkDepth = GroundHit.ImpactPoint.Z - (Location.Z - ExtentHeight);

	return CurrentSinkDepth > GroundDigDepth;
}

bool UConstructionComponent::HasGround(const FVector& Location, FHitResult& GroundHit, const float ExtentHeight) {
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.AddIgnoredActor(GhostActor);

	FVector Start = Location + FVector(0.f, 0.f, 50.f);
	FVector End = Location - FVector(0.f, 0.f, (ExtentHeight + MaxFloatingDistance));

	return GetWorld()->LineTraceSingleByChannel(GroundHit, Start, End, CollisionChannels::BuildableGround, Params);
}

bool UConstructionComponent::CheckPlacement(FVector& Location) {
	if (!CurrentBuildData || !GhostActor) return false;

	UStaticMeshComponent* GhostComp = GhostActor->GetStaticMeshComponent();
	if (!GhostComp || !GhostComp->GetStaticMesh()) return false;

	const float ExtentHeight = GhostComp->GetStaticMesh()->GetBounds().BoxExtent.Z;
	
	FHitResult GroundHit;

	if (!HasGround(Location, GroundHit, ExtentHeight)) return false;
	if (IsTooDeep(Location, GroundHit, ExtentHeight)) return false;

	AlignPlacement(Location, GroundHit, ExtentHeight);

	if (IsBlocked(FinalSnappedLocation, GhostComp)) return false;

	return true;
}

void UConstructionComponent::AlignPlacement(FVector& Location, const FHitResult& GroundHit, const float ExtentHeight) {
	FVector FacetBottom = Location - FVector(0.f, 0.f, ExtentHeight);
	FVector Offset = GroundHit.ImpactPoint - FacetBottom;

	FinalSnappedLocation = Location + Offset + FVector(0.f, 0.f, 0.1f);
}

void UConstructionComponent::RotateBuildObject(float Direction) {
	if (!bBuildModeActive || !CurrentBuildData || !GhostActor) return;

	RotateTarget.Yaw += RotateRate * Direction;
	RotateTarget.Normalize();

	if (bGhostMeshReady) {
		UpdateGhost(LastSnappedLocation);
	}
}

void UConstructionComponent::TryPlaceBuildObject() {
	if (!bCanPlace || !CurrentBuildData) return;

	if (UClass* ClassToSpawn = CurrentBuildData->ActorClass.Get()) {
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(ClassToSpawn, FinalSnappedLocation, RotateTarget, Params);

		bCanPlace = CheckPlacement(FinalSnappedLocation);
		UpdateGhostMaterial();
	}
}

void UConstructionComponent::PlaceBuildObject() {
	if (!CurrentBuildData || PreviousBuildData != CurrentBuildData) return;

	if (!CurrentBuildData->ActorClass.IsValid()) {
		LoadBuildClassAsync();
		return;
	}

	TryPlaceBuildObject();
}

void UConstructionComponent::PlaceBuildObjectOnLoaded(TWeakObjectPtr<UBuildObjectData> LoadedData) {
	if (CurrentBuildData != LoadedData) return;

	TryPlaceBuildObject();
}

void UConstructionComponent::LoadBuildClassAsync() {
	if (!CurrentBuildData || CurrentBuildData->ActorClass.IsNull()) return;

	TWeakObjectPtr<UConstructionComponent> WeakThis(this);
	TWeakObjectPtr<UBuildObjectData> WeakObject(CurrentBuildData);

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		CurrentBuildData->ActorClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda([WeakThis, WeakObject]() {
			if (WeakThis.IsValid()) {
				WeakThis->PlaceBuildObjectOnLoaded(WeakObject);
			}
			})
		);
}

void UConstructionComponent::SelectBuildingFromLibrary(int32 IndexToSelect) {
	if (!BuildLibrary.IsValid() || !BuildLibrary.Get()->AllBuildings.IsValidIndex(IndexToSelect)) return;

	LoadBuildDataAsync(IndexToSelect);
}

void UConstructionComponent::SetBuildLibraryData(UBuildLibraryData* NewData) {
	if (!NewData) return;

	BuildLibrary = NewData;
}

void UConstructionComponent::ToggleBuildMode(bool bEnable) {
	if (!bEnable) {
		CurrentBuildData = nullptr;
		PreviousBuildData = nullptr;
		bCanPlace = false;
		bGhostMeshReady = false;
		CurrentIndexToLoad = -1;

		SetComponentTickEnabled(false);

		if (GhostActor) {
			GhostActor->SetActorHiddenInGame(true);
			GhostActor->GetStaticMeshComponent()->SetStaticMesh(nullptr);
		}
	}
	else {
		SetComponentTickEnabled(true);
	}

	bBuildModeActive = bEnable;
}

void UConstructionComponent::UpdateGhostMaterial() {
	if (!CurrentBuildData || !GhostActor || !HologramBaseMaterial || !GhostDMI.IsValid()) return;

	FLinearColor TargetColor = bCanPlace ? FLinearColor::Green : FLinearColor::Red;
	float TargetOpacity = bCanPlace ? 0.5f : 0.8f;

	GhostDMI.Get()->SetVectorParameterValue(TEXT("Color"), TargetColor);
	GhostDMI.Get()->SetScalarParameterValue(TEXT("Opacity"), TargetOpacity);
}

void UConstructionComponent::OnBuildDataLoaded(int32 IndexToLoad) {
	if (!BuildLibrary.IsValid() || CurrentIndexToLoad < 0 || CurrentIndexToLoad != IndexToLoad) return;

	TSoftObjectPtr<UBuildObjectData> LoadedSoftPtr = BuildLibrary->AllBuildings[IndexToLoad];

	if (UBuildObjectData* Data = LoadedSoftPtr.Get()) {
		CurrentBuildData = Data;
	}
}

void UConstructionComponent::LoadBuildDataAsync(int32 IndexToLoad) {
	if (!BuildLibrary.IsValid() || !BuildLibrary.Get()->AllBuildings.IsValidIndex(IndexToLoad)) return;

	TSoftObjectPtr<UBuildObjectData> SoftData = BuildLibrary->AllBuildings[IndexToLoad];

	CurrentIndexToLoad = IndexToLoad;

	if (SoftData.IsValid()) {
		OnBuildDataLoaded(IndexToLoad);
		return;
	}

	TWeakObjectPtr<UConstructionComponent> WeakThis(this);
	const int32 CachedObjectIndex = IndexToLoad;

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		BuildLibrary->AllBuildings[IndexToLoad].ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda([WeakThis, CachedObjectIndex]() {
			if (WeakThis.IsValid()) {
				WeakThis->OnBuildDataLoaded(CachedObjectIndex);
			}
		})
	);
}

void UConstructionComponent::OnGhostMeshLoaded(TWeakObjectPtr<UBuildObjectData> LoadedData) {
	UBuildObjectData* Data = LoadedData.Get();

	if (!Data || LoadedData != PreviousBuildData || !GhostActor || !Data->GhostMesh.IsValid()) return;

	GhostActor->GetStaticMeshComponent()->SetStaticMesh(Data->GhostMesh.Get());

	if (HologramBaseMaterial) {
		GhostDMI = GhostActor->GetStaticMeshComponent()->CreateDynamicMaterialInstance(0, HologramBaseMaterial);
	}

	bGhostMeshReady = true;
}

void UConstructionComponent::LoadGhostMeshAsync() {
	if (!CurrentBuildData || CurrentBuildData->GhostMesh.IsNull()) return;
	
	if (CurrentBuildData->GhostMesh.IsValid()) {
		OnGhostMeshLoaded(CurrentBuildData);
		return;
	}

	TWeakObjectPtr<UConstructionComponent> WeakThis(this);
	TWeakObjectPtr<UBuildObjectData> WeakObject(CurrentBuildData);

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		CurrentBuildData->GhostMesh.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda([WeakThis, WeakObject]() {
			if (WeakThis.IsValid()) {
				WeakThis->OnGhostMeshLoaded(WeakObject);
			}
		})
	);
}

void UConstructionComponent::UpdateGhost(FVector& ViewLocation) {
	if (!CurrentBuildData || !bBuildModeActive) return;

	if (!GhostActor) {
		GhostActor = GetWorld()->SpawnActor<AStaticMeshActor>();
		GhostActor->SetMobility(EComponentMobility::Movable);

		if (UStaticMeshComponent* SMC = GhostActor->GetStaticMeshComponent()) {
			SMC->SetSimulatePhysics(false);
			SMC->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}

	if (PreviousBuildData != CurrentBuildData) {
		PreviousBuildData = CurrentBuildData;

		bGhostMeshReady = false;

		LoadGhostMeshAsync();
	}

	if (!bGhostMeshReady) {
		GhostActor.Get()->SetActorHiddenInGame(true);
		return;
	}

	GhostActor->SetActorHiddenInGame(false);
	GhostActor->SetActorLocation(ViewLocation);
	GhostActor->SetActorRotation(RotateTarget);

	bCanPlace = CheckPlacement(ViewLocation);
	UpdateGhostMaterial();
}

void UConstructionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner()) {
		PlayerCamera = Owner->FindComponentByClass<UCameraComponent>();
	}
}

void UConstructionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bBuildModeActive && CurrentBuildData && PlayerCamera.Get()) {
		FVector Snapped = GetPlacementLocation();

		if (!Snapped.Equals(LastSnappedLocation)) {
			LastSnappedLocation = Snapped;
			UpdateGhost(Snapped);
		}
	}
}

