// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "ExaminatableInterface.h"
#include "PickUpActor.generated.h"

class UItemDefinition;
struct FExamineData;

UCLASS()
class FPS_PROJECT_API APickUpActor : public AActor, public IInteractableInterface, public IExaminatableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpActor();

	void Interact_Implementation(AActor* Interactor);
	virtual UPrimitiveComponent* GetPrimitiveOnInteract() override;
	virtual FExamineItem* GetExaminationItem() override;

	UItemDefinition* GetItemDef() { return ItemDef; }
	int32 GetAmount() { return Amount; }

	void ThrowItem(FVector Direction);

	void SetItemDef(UItemDefinition* NewItemDef);
	void SetAmount(int32 NewAmount);

protected:
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Initialize();

private:
	UPROPERTY()
		TObjectPtr<AActor> VisualActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Logic")
		TObjectPtr<UItemDefinition> ItemDef = nullptr;

	UPROPERTY(EditAnywhere, Category = "Logic")
		int32 Amount = 1;
};
