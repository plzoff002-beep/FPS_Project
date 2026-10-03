// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttachmentInterface.h"
#include "AttachmentTypes.h"
#include "LaserAttachment.generated.h"

class UNiagaraComponent;

UCLASS()
class FPS_PROJECT_API ALaserAttachment : public AActor, public IAttachmentInterface
{
	GENERATED_BODY()
	
public:	
	ALaserAttachment();

	virtual void SetActorHiddenInGame(bool bNewHidden) override;

	virtual void ToggleAttachment_Implementation() override;

	UPROPERTY(EditAnywhere, Category = "Visual")
		TObjectPtr<UStaticMeshComponent> LaserMesh;

	UPROPERTY(EditAnywhere, Category = "Visual")
		TObjectPtr<UNiagaraComponent> LaserBeamComponent;

	UPROPERTY(EditAnywhere, Category = "Laser Settings")
		float MaxDist = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Laser Settings")
		FName LaserSocket = "SOCKET_Origin";

	virtual void Tick(float DeltaTime) override;
protected:
	bool bIsLaserActive = true;

};
