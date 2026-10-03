#pragma once

#include "CoreMinimal.h"
#include "WeaponIKTypes.generated.h"

USTRUCT(BlueprintType)
struct FWeaponIKData {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
		FTransform LeftHandTarget = FTransform::Identity;

	UPROPERTY(BlueprintReadOnly)
		float IKAlpha = 0.f;

	bool HasGrip = false;
};

UINTERFACE(MinimalAPI)
class UWeaponIKProvider : public UInterface { GENERATED_BODY() };

class IWeaponIKProvider {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
		FWeaponIKData GetWeaponIKData() const;

	UFUNCTION(BlueprintNativeEvent)
		FTransform GetRightHandIKTransform() const;
};
