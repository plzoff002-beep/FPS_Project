#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();

	void EndInteraction(const float ElapsedTime);

	void TryGrabObject();
	void TryPickUpObject();
	void TryExamineItem(bool& bExamed);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Interaction")
		float InteractionMaxDistance = 250.f;

private:
	TObjectPtr<AActor> OwnerActor = nullptr;

	bool PerformInteractionTrace(FHitResult& Hit);
};
