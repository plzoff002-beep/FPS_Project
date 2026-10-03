// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "ViewModelComponent.h"
#include "FPSWidget.h"
#include "WeaponManager.h"
#include "WeaponBase.h"
#include "FPSAnimInstance.h"
#include "InteractableInterface.h"
#include "WeaponAttachmentComponent.h"
#include "PickUpActor.h"
#include "ItemFragment.h"
#include "ItemDefinition.h"
#include "DrawDebugHelpers.h"
#include "LocalUIManager.h"
#include "UITypes.h"
#include "UIData.h"
#include "LocalGlobalSettings.h"
#include "FPSGlobalSettingsSave.h"
#include "SettingsDelegates.h"
#include "UISlotIndexReceiver.h"
#include "StaminaComponent.h"
#include "SprintComponent.h"
#include "HandleGrabComponent.h"
#include "ConstructionComponent.h"
#include "ExaminationComponent.h"
#include "ExaminatableInterface.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	Pivot->SetupAttachment(RootComponent);
	Pivot->SetRelativeLocation(FVector(0, 0, 64.f));

	CharacterArms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterArms"));
	CharacterArms->SetupAttachment(Pivot);
	CharacterArms->SetRelativeLocation(FVector(0, 0, 64.f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;

	ViewModel = CreateDefaultSubobject<UViewModelComponent>(TEXT("ViewModelComponent"));
	WeaponManager = CreateDefaultSubobject<UWeaponManager>(TEXT("WeaponManager"));
	SprintComponent = CreateDefaultSubobject<USprintComponent>(TEXT("SprintComponent"));
	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	HandleGrabComponent = CreateDefaultSubobject<UHandleGrabComponent>(TEXT("HandleGrabComponent"));
	ConstructionComponent = CreateDefaultSubobject<UConstructionComponent>(TEXT("ConstructionComponent"));
	ExaminationComponent = CreateDefaultSubobject<UExaminationComponent>(TEXT("ExaminationComponent"));
}

FTransform AFPSCharacter::GetRightHandIKTransform_Implementation() const {
	if (!CharacterArms) {
		return FTransform::Identity;
	}

	return CharacterArms->GetSocketTransform(FName("ik_hand_gun"), RTS_World);
}

FVector AFPSCharacter::GetCameraRelativeLocation() const {
	if (!Camera) {
		return FVector::ZeroVector;
	}

	return Camera->GetRelativeLocation();
}

FViewModelState AFPSCharacter::GetViewModelState_Implementation() const {
	if (!ViewModel) return {};

	return ViewModel->GetState();
}

FCharacterState AFPSCharacter::GetCharacterState_Implementation() const {
	FCharacterState Result;

	Result.Velocity = GetVelocity().Size2D();

	Result.bInAir = IsFalling();

	Result.bWalking = IsWalking() && !IsFalling();

	Result.bSprinting = SprintComponent->IsSprinting();

	Result.bCrouching = IsCrouching() && !IsFalling();

	Result.bAiming = IsAiming();

	Result.bCloseToWall = IsCloseToWall();

	return Result;
}

void AFPSCharacter::UnifiedUIHandler(EButtonType ButtonType, UObject* Context) {
	if (!UIManager) return;

	switch (ButtonType) {
	case EButtonType::Drop:
		if (IUISlotIndexReceiver* SlotReceiver = Cast<IUISlotIndexReceiver>(Context)) {
			HandleDropClickedItem(SlotReceiver->GetSlotIndex());
		}
		break;

	case EButtonType::Delete:
		if (IUISlotIndexReceiver* SlotReceiver = Cast<IUISlotIndexReceiver>(Context)) {
			Inventory->RemoveItem(SlotReceiver->GetSlotIndex());
		}
		break;
	}
}

void AFPSCharacter::SetPlayerGameplayConfig(FGameplaySettingsData& NewData) {
	PlayerGameplayConfig.MouseSensivity = NewData.MouseSensivity;
	PlayerGameplayConfig.FieldOfView = NewData.FieldOfView;
	PlayerGameplayConfig.bToggleToSprint = NewData.bToggleToSprint;
}

bool AFPSCharacter::IsCloseToWall() const {
	FVector Origin = Camera->GetComponentLocation();
	FVector Direction = Camera->GetForwardVector();
	FVector End = Origin + Direction * DistanceToWall;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	return GetWorld()->LineTraceTestByChannel(Origin, End, ECC_Visibility, Params);
}

bool AFPSCharacter::IsFalling() const {
	return GetCharacterMovement()->IsFalling();
}

bool AFPSCharacter::IsReloading() const {
	return WeaponManager->IsReloading();
}

bool AFPSCharacter::IsSprinting() const {
	if (!SprintComponent && !IsFalling()) return false;

	return SprintComponent->IsSprinting();
}

FWeaponIKData AFPSCharacter::GetWeaponIKData_Implementation() const {
	if (!WeaponManager || !WeaponManager->GetCurrentWeapon()) {
		FWeaponIKData EmptyData{};

		EmptyData.IKAlpha = 0.f;
		EmptyData.HasGrip = false;

		return EmptyData;
	}

	return WeaponManager->GetCurrentWeapon()->GetWeaponIKData();
}

bool AFPSCharacter::TryThrowGrabbedObject() {
	if (!HandleGrabComponent) return false;

	return HandleGrabComponent->ThrowObject();
}

bool AFPSCharacter::TryReleaseGrabbedObject() {
	if (!HandleGrabComponent) return false;

	return HandleGrabComponent->ReleaseObject();
}

void AFPSCharacter::ToggleExamination(bool& bExamed) {
	if (!ExaminationComponent) return;

	if (!bExamed) {
		if (FHitResult* Hit = PerformInteraction()) {
			if (AActor* HitActor = Hit->GetActor()) {
				if (IExaminatableInterface* Examinatable = Cast<IExaminatableInterface>(HitActor->GetOwner())) {
					ExaminationComponent->StartExamination(Examinatable->GetExaminationItem());
					bExamed = true;
				}
			}
		}
	}
	else {
		ExaminationComponent->StopExamination();
		bExamed = false;
	}
}

void AFPSCharacter::RotateBuildObject(float Direction) {
	if (!ConstructionComponent) return;

	ConstructionComponent->RotateBuildObject(Direction);
}

void AFPSCharacter::PlaceBuildObject() {
	if (!ConstructionComponent) return;

	ConstructionComponent->PlaceBuildObject();
}

void AFPSCharacter::ToggleBuildMode(bool bEnable) {
	if (!ConstructionComponent) return;

	ConstructionComponent->ToggleBuildMode(bEnable);
}

void AFPSCharacter::SelectBuildingFromLibrary(int32 IndexToSelect) {
	if (!ConstructionComponent) return;

	ConstructionComponent->SelectBuildingFromLibrary(IndexToSelect);
}

void AFPSCharacter::BeginInteraction() {
	if (!bHoldingObject) {
		bHoldingObject = true;

		TryGrabObject();
	}
}

void AFPSCharacter::EndInteraction(float ElapsedTime) {
	if (ElapsedTime < 0.2f) {
		TryReleaseGrabbedObject();
		TryPickUpObject();

		bHoldingObject = false;
	}
}

void AFPSCharacter::TryGrabObject() {
	if (FHitResult* Hit = PerformInteraction()) {
		if (AActor* HitActor = Hit->GetActor()) {
			if (IInteractableInterface* Interactable = Cast<IInteractableInterface>(HitActor->GetOwner())) {
				UPrimitiveComponent* TargetToGrab = Interactable->GetPrimitiveOnInteract();

				HandleGrabComponent->GrabObject(this, TargetToGrab, Hit->ImpactPoint);
			}
		}
	}
}

void AFPSCharacter::HandleSlotCleared(int32 SlotIndex) {
	if (!Inventory || !WeaponManager) return;

	if (SlotIndex == WeaponManager->GetCurrentWeaponIndex()) {
		WeaponManager->ImmediateUnEquip();
	}
}

void AFPSCharacter::HandleInventoryChanged(int32 SlotIndex) const {
	FInventorySlot* Slot = Inventory->GetSlot(SlotIndex);
	
	if (Slot && !Slot->IsEmpty()) {
		auto* WeaponFragment = Slot->ItemDef->FindFragment<UItemFragment_Weapon>();

		if (WeaponFragment) {
			if (AWeaponBase* SpawnedActor = WeaponManager->SpawnWeapon(CharacterArms, WeaponFragment)) {
				Inventory->RegisterActorInSlot(SlotIndex, SpawnedActor);
			}
		}
	}
}

void AFPSCharacter::SetDropLocation(FInventorySlot& ItemToDrop) {
	if (!ItemToDrop.IsEmpty()) {
		FVector SpawnLoc = Camera->GetComponentLocation() + (Camera->GetForwardVector() * 200.f);
		FRotator SpawnRot = Camera->GetComponentRotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		APickUpActor* NewPickUp = GetWorld()->SpawnActorDeferred<APickUpActor>(APickUpActor::StaticClass(), FTransform(SpawnRot, SpawnLoc));

		if (NewPickUp) {
			NewPickUp->SetItemDef(ItemToDrop.ItemDef);
			NewPickUp->SetAmount(ItemToDrop.Quantity);

			NewPickUp->FinishSpawning(FTransform(SpawnRot, SpawnLoc));

			FVector Direction = Camera->GetForwardVector() * 400.f;

			NewPickUp->ThrowItem(Direction);
		}
	}
}

void AFPSCharacter::HandleDropClickedItem(int32 SlotIndex) {
	if (!Inventory || !WeaponManager) return;

	FInventorySlot ItemToDrop = Inventory->RemoveItem(SlotIndex);

	if (!ItemToDrop.IsEmpty()) {
		SetDropLocation(ItemToDrop);
	}
}

void AFPSCharacter::DropItem() {
	if (!Inventory || !WeaponManager) return;

	int32 IndexToDrop = WeaponManager->GetCurrentWeaponIndex();
	if (IndexToDrop == -1) IndexToDrop = 0;

	FInventorySlot ItemToDrop = Inventory->RemoveItem(IndexToDrop);
	
	if (!ItemToDrop.IsEmpty()) {
		SetDropLocation(ItemToDrop);
	}
}

void AFPSCharacter::TryPickUpObject() {
	if (FHitResult* Hit = PerformInteraction()) {
		if (AActor* HitActor = Hit->GetActor()) {
			AActor* PickUpActor = HitActor->GetOwner();

			if (PickUpActor && PickUpActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass())) {
				IInteractableInterface::Execute_Interact(PickUpActor, this);
			}
		}
	}
}

FHitResult* AFPSCharacter::PerformInteraction() {
	FVector Origin = Camera->GetComponentLocation();
	FVector Direction = Camera->GetForwardVector();
	FVector End = Origin + Direction * InteractionMaxDistance;

	FHitResult Hit;

	FCollisionQueryParams Params;

	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Origin, End, ECC_Visibility, Params)) {
		return &Hit;
	}

	return nullptr;
}

void AFPSCharacter::MoveWeaponStock() const {
	if (!WeaponManager) return;

	WeaponManager->TryMoveWeaponStock();
}

void AFPSCharacter::SetCameraRelativeLocation(FVector NewLocation) {
	Camera->SetRelativeLocation(NewLocation);
}

void AFPSCharacter::SwapWeapon(const float Index) const {
	if (!WeaponManager || !Inventory) return;

	const int32 IntIndex = FMath::RoundToInt32(Index - 1);

	FInventorySlot* Slot = Inventory->GetSlot(IntIndex);

	if (Slot && !Slot->IsEmpty()) {
		if (AActor* SpawnedActor = Slot->SpawnedActor.Get()) {
			WeaponManager->SwapWeapon(IntIndex, SpawnedActor);
		}
	}
}

void AFPSCharacter::ToggleLaserAttachments() const {
	WeaponManager->GetCurrentWeapon()->GetWeaponAttachmentComponent()->TryToggleLaserAttachments();
}

void AFPSCharacter::Reload() const {
	if (!WeaponManager || IsCloseToWall() || IsSprinting()) return;

	WeaponManager->TryReload();
}

void AFPSCharacter::Shoot() const {
	if (WeaponManager && WeaponManager->GetCurrentWeapon() && !IsReloading() && !IsCloseToWall() && (!SprintComponent || !SprintComponent->IsSprinting())) {
		WeaponManager->TryShoot();
	}
}

void AFPSCharacter::SetWalkState(bool bValue) {
	bWalk = bValue;
}

void AFPSCharacter::SetSprintState(bool bValue) {
	if (!SprintComponent) return;

	SprintComponent->SetWantToSprint(bValue);
}

void AFPSCharacter::SetCrouchState(bool bValue) {
	bCrouch = bValue;
}

void AFPSCharacter::SetAimState(bool bValue) {
	bAim = bValue;
}

void AFPSCharacter::UpdateFOV(float DeltaTime) {
	if (!Camera) return;

	const float TargetFOV = SprintComponent->IsSprinting() ? SprintFOV : PlayerGameplayConfig.FieldOfView;

	Camera->FieldOfView = FMath::FInterpTo(
		Camera->FieldOfView,
		TargetFOV, 
		DeltaTime, 
		FOVAccel
	);
}

void AFPSCharacter::UpdateCrouch(float DeltaTime) {
	const float HeightTarget = bCrouch ? CrouchHalfHeight : StandHalfHeight;
	const float CurrentHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	const float NewHeight = FMath::FInterpTo(
		CurrentHeight, 
		HeightTarget, 
		DeltaTime, 
		HeightAccel
	);

	GetCapsuleComponent()->SetCapsuleHalfHeight(NewHeight);
}

//void AFPSCharacter::UpdateMovementSpeed(const float DeltaTime) {
//	const float TargetSpeed = bSprint && !IsReloading() && StaminaComponent->GetStamina() > 0 ? SprintSpeed : WalkSpeed;
//	float& CurrentSpeed = GetCharacterMovement()->MaxWalkSpeed;
//
//	const float NewSpeed = FMath::FInterpTo(
//		CurrentSpeed,
//		TargetSpeed, 
//		DeltaTime, 
//		SpeedAccel
//	);
//
//	CurrentSpeed = NewSpeed;
//}

void AFPSCharacter::MoveCharacter(const FVector2D& Axis) {
	AddMovementInput(GetActorForwardVector(), Axis.Y);
	AddMovementInput(GetActorRightVector(), Axis.X);
}

void AFPSCharacter::Look(const FVector2D& Axis) {
	MouseInputThisFrame = Axis * -1.f;

	AddControllerYawInput(Axis.X * PlayerGameplayConfig.MouseSensivity);
	AddControllerPitchInput(Axis.Y * PlayerGameplayConfig.MouseSensivity);

	FRotator PivotRot = Pivot->GetRelativeRotation();

	PivotRot.Pitch = GetControlRotation().Pitch;

	Pivot->SetRelativeRotation(PivotRot * -1.f);
}

void AFPSCharacter::ToggleInventory() {
	if (!UIManager) return;

	if (!bIsInventoryOpen) {
		UIManager->FindAndLoadWidget<UInventoryMenuWidget>(nullptr);

		if (UInventoryMenuWidget* MenuWidget = UIManager->GetWidget<UInventoryMenuWidget>()) {
			MenuWidget->BindToInventory(Inventory);
		}

		bIsInventoryOpen = true;
	}
	else {
		if (UIManager->FindAndPopWidget<UInventoryMenuWidget>()) {
			UIManager->FindAndPopWidget<UContextMenu>();

			bIsInventoryOpen = false;
		}
	}
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (ConstructionComponent && BuildLibraryData.Get()) {
		ConstructionComponent->SetBuildLibraryData(BuildLibraryData.Get());
	}

	if (CharacterStatsData) {
		if (SprintComponent) {
			SprintComponent->InitFromDataAsset(CharacterStatsData);
		}

		if (StaminaComponent) {
			StaminaComponent->InitFromDataAsset(CharacterStatsData);
		}
	}

	if (SprintComponent && StaminaComponent) {
		SprintComponent->SetStaminaComponent(StaminaComponent);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
		UIManager = PC->GetLocalPlayer()->GetSubsystem<ULocalUIManager>();

		if (UIManager) {
			UIManager->SetUIData(UIData);

			UIManager->OnGameplayActionRequest.AddUObject(this, &AFPSCharacter::UnifiedUIHandler);
		}

		if (ULocalGlobalSettings* GlobalSettings = PC->GetLocalPlayer()->GetSubsystem<ULocalGlobalSettings>()) {
			GlobalSettings->SetSystemData(SystemData);

			GlobalSettings->OnGameplaySettingsChanged.AddUObject(this, &AFPSCharacter::SetPlayerGameplayConfig);
		}
	}

	Camera->AttachToComponent(
		CharacterArms,
		FAttachmentTransformRules::KeepRelativeTransform,
		CameraSocket
	);

	WeaponManager->OnWeaponChanged.AddUObject(
		ViewModel,
		&UViewModelComponent::OnWeaponChanged
	);

	WeaponManager->OnWeaponMontageStarted.AddUObject(
		ViewModel,
		&UViewModelComponent::HandleWeaponMontageEvent
	);

	ViewModel->OnMontageFinished.AddUObject(
		WeaponManager,
		&UWeaponManager::HandleMontageFinished
	);

	Inventory->OnInventoryChanged.AddUObject(
		this,
		&AFPSCharacter::HandleInventoryChanged
	);

	Inventory->OnInventorySlotCleared.AddUObject(
		this,
		&AFPSCharacter::HandleSlotCleared
	);
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UpdateMovementSpeed(DeltaTime);
	UpdateCrouch(DeltaTime);
	UpdateFOV(DeltaTime);

	/*MouseInputThisFrame = FVector2D::ZeroVector;*/
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

