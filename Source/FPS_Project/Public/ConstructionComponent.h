#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildLibraryData.h"
#include "ConstructionComponent.generated.h"

class UBuildObjectData;
class AStaticMeshActor;
class USceneComponent;
class UMaterialInterface;
class UMaterialDynamicInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API UConstructionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UConstructionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ToggleBuildMode(bool bEnable);
	void SetBuildLibraryData(UBuildLibraryData* NewData);

	void SelectBuildingFromLibrary(int32 IndexToSelect);
	void PlaceBuildObject();
	void RotateBuildObject(float Direction);

protected:
	virtual void BeginPlay() override;

private:
	void UpdateGhost(FVector& ViewLocation);
	void UpdateGhostMaterial();

	void LoadGhostMeshAsync();
	void OnGhostMeshLoaded(TWeakObjectPtr<UBuildObjectData> LoadedData);

	void LoadBuildDataAsync(int32 IndexToLoad);
	void OnBuildDataLoaded(int32 IndexToLoad);

	void LoadBuildClassAsync();
	
	void PlaceBuildObjectOnLoaded(TWeakObjectPtr<UBuildObjectData> LoadedData);
	void TryPlaceBuildObject();

	void AlignPlacement(FVector& Location, const FHitResult& GroundHit, const float ExtentHeight);

	bool CheckPlacement(FVector& Location);
	bool HasGround(const FVector& Location, FHitResult& GroundHit, const float ExtentHeight);
	bool IsTooDeep(const FVector& Location, const FHitResult& GroundHit, const float ExtentHeight);
	bool IsBlocked(const FVector& Location, UStaticMeshComponent* MeshComp);

	bool bCanPlace = false;
	bool bBuildModeActive = false;

	bool bGhostMeshReady = false;

	FVector GetPlacementLocation();

	UPROPERTY(EditAnywhere, Category = "Build | Placing")
		float MaxBuildDistance = 250.f;

	UPROPERTY(EditAnywhere, Category = "Build | Placing")
		float RotateRate = 15.f;

	UPROPERTY(EditAnywhere, Category = "Build | Validation")
		float GroundDigDepth = 20.f;

	UPROPERTY(EditAnywhere, Category = "Build | Validation")
		float MaxFloatingDistance = 10.f;

	int32 CurrentIndexToLoad = -1;

	FVector LastSnappedLocation = FVector::ZeroVector;
	FVector FinalSnappedLocation = FVector::ZeroVector;
	FRotator RotateTarget = FRotator::ZeroRotator;

	TWeakObjectPtr<USceneComponent> PlayerCamera = nullptr;
	UPROPERTY() TObjectPtr<AStaticMeshActor> GhostActor = nullptr;

	TWeakObjectPtr<UBuildLibraryData> BuildLibrary = nullptr;
	UPROPERTY() TObjectPtr<UBuildObjectData> CurrentBuildData = nullptr;
	TWeakObjectPtr<UBuildObjectData> PreviousBuildData = nullptr;

	TWeakObjectPtr<UMaterialInstanceDynamic> GhostDMI = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Build Visual")
		TObjectPtr<UMaterialInterface> HologramBaseMaterial = nullptr;
};
