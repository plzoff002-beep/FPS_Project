#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuildObjectData.h"
#include "BuildLibraryData.generated.h"

UCLASS()
class FPS_PROJECT_API UBuildLibraryData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Buildings")
		TArray<TSoftObjectPtr<UBuildObjectData>> AllBuildings;
};
