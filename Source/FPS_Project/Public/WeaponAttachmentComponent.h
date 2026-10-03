// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttachmentTypes.h"
#include "WeaponAttachmentComponent.generated.h"

class UAttachmentData;
class AWeaponBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_PROJECT_API UWeaponAttachmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponAttachmentComponent();

	void Initialize();

	void SetAttachmentsHiddenInGame(bool bNewHidden);

	void AddAttachment(const FAttachmentRunTimeData& NewAttachment);

	void ToggleStockState(EAttachmentState NewState);

	void TryToggleLaserAttachments();

	bool IsStockExtended() const;

	bool HasGripAttachment() const;
	bool HasStockAttachment() const;

	FTransform GetGripSocketTransform() const;

	FWeaponStatModifiers GetTotalModifiers() const;

	UPROPERTY(EditAnywhere, Category = "Weapon | Attachments")
		TMap<EAttachmentType, FAttachmentRunTimeData> CurrentAttachments;

	TMap <EAttachmentType, UStaticMeshComponent*> AttachmentComponents;
	TMap <EAttachmentType, AActor*> SpawnedAttachmentActors;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
