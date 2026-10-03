#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExaminationComponent.generated.h"

class AExamineStudio;
class ULocalUIManager;

struct FInputActionValue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API UExaminationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UExaminationComponent();

	void SetupStudio();

	void RotateItem(const FInputActionValue& Value);
	void AdjustSceneCamera(const FInputActionValue& Value);

	void StartExamination(FExamineItem* Data);
	void StopExamination();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Examination Settings")
		TSoftClassPtr<AExamineStudio> StudioClass = nullptr;

private:
	UPROPERTY() TObjectPtr<AActor> PreviousViewTarget = nullptr;
	UPROPERTY() TObjectPtr<AExamineStudio> CurrentStudio = nullptr;
	UPROPERTY() TObjectPtr<ULocalUIManager> UIManager = nullptr;

	void LoadStudioClassAsync();
	void SetupStudioOnLoaded();
};
