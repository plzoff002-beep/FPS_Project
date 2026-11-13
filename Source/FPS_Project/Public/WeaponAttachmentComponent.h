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
	UWeaponAttachmentComponent();

	void Initialize();

	void SetAttachmentsHiddenInGame(bool bNewHidden);

	void AddAttachment(const FAttachmentRunTimeData& NewAttachment);
	void ToggleStockState(EAttachmentState NewState);
	void TryToggleLaserAttachments();

	bool IsStockExtended() const;

	bool HasGripAttachment() const;
	bool HasStockAttachment() const;
	bool IsAttachmentComponentsEmpty() const { return AttachmentComponents.IsEmpty(); }
	bool IsCurrentAttachmentsEmpty() const { return CurrentAttachments.IsEmpty(); }

	const TMap<EAttachmentType, FAttachmentRunTimeData>& GetCurrentAttachments() const { return CurrentAttachments; }

	UStaticMeshComponent* GetAttachmentMesh(EAttachmentType TargetType) const { return AttachmentComponents.FindRef(TargetType); }

	FTransform GetGripSocketTransform() const;

	FWeaponStatModifiers GetTotalModifiers() const;

protected:
	UPROPERTY(EditAnywhere, Category = "Weapon | Attachments")
	TMap<EAttachmentType, FAttachmentRunTimeData> CurrentAttachments;

	TMap <EAttachmentType, UStaticMeshComponent*> AttachmentComponents;
	TMap <EAttachmentType, AActor*> SpawnedAttachmentActors;
};
