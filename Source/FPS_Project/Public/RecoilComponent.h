#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RecoilComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API URecoilComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URecoilComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetPlayerController(APlayerController* NewPC);
	void AddRecoil(const float Value);

protected:
	virtual void BeginPlay() override;
		
	UPROPERTY(EditAnywhere, Category = "Recoil")
		float RecoilAccel = 15.f;

	void ResetRecoilComponent();

private:
	TWeakObjectPtr<APlayerController> PC = nullptr;

	FRotator TargetRecoilOffset = FRotator::ZeroRotator;
	FRotator AppliedRecoilOffset = FRotator::ZeroRotator;
};
