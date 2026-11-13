#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UITypes.h"
#include "UIData.generated.h"

UCLASS()
class FPS_PROJECT_API UUIData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "UI")
		TArray<FWidgetData> WidgetClasses;

	FWidgetData* FindWidget(EUIType TargetType);
};
