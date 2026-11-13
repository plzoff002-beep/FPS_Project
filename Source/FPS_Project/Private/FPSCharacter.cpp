#include "FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "ViewModelComponent.h"
#include "WeaponManager.h"
#include "WeaponBase.h"
#include "FPSAnimInstance.h"
#include "InteractableInterface.h"
#include "PickUpActor.h"
#include "ItemFragment.h"
#include "ItemDefinition.h"
#include "DrawDebugHelpers.h"
#include "LocalUIManager.h"
#include "UIData.h"
#include "LocalGlobalSettings.h"
#include "FPSGlobalSettingsSave.h"
#include "UISlotIndexReceiver.h"
#include "StaminaComponent.h"
#include "SprintComponent.h"
#include "HandleGrabComponent.h"
#include "ConstructionComponent.h"
#include "ExaminationComponent.h"
#include "InteractionComponent.h"
#include "FPSStateComponent.h"
#include "CrouchComponent.h"
#include "FieldOfViewComponent.h"
#include "RecoilComponent.h"
#include "ContextMenu.h"

AFPSCharacter::AFPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

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
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	CrouchComponent = CreateDefaultSubobject<UCrouchComponent>(TEXT("CrouchComponent"));
	StateComponent = CreateDefaultSubobject<UFPSStateComponent>(TEXT("StateComponent"));
	FOVComp = CreateDefaultSubobject<UFieldOfViewComponent>(TEXT("FieldOfViewComponent"));
	RecoilComponent = CreateDefaultSubobject<URecoilComponent>(TEXT("RecoilComponent"));
}

FTransform AFPSCharacter::GetRightHandIKTransform() {
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

FViewModelState AFPSCharacter::GetViewModelState() {
	if (!ViewModel) return {};

	return ViewModel->GetState();
}

void AFPSCharacter::ItemSpawnerHandler(UItemDefinition* ItemDef, const int32 SlotIndex) {
	if (!ItemDef) {
		UE_LOG(LogTemp, Error, TEXT("FPSCharacter: ItemSpawnerHandler is Failed"));
		return;
	}

	TArray<TObjectPtr<UItemFragment>>& Fragments = ItemDef->Fragments;

	for (UItemFragment* Fragment : Fragments) {
		if (UItemFragment_Weapon* WeaponFragment = Cast<UItemFragment_Weapon>(Fragment)) {
			WeaponManager->LoadWeaponAsync(WeaponFragment->WeaponClass, SlotIndex, CharacterArms.Get());
		}
	}
}

void AFPSCharacter::ItemSelectionHandler(const int32 Index, AActor* SpawnedActor, EItemType ItemType) {
	UE_LOG(LogTemp, Warning, TEXT("4"));

	if (!SpawnedActor) return;

	UE_LOG(LogTemp, Warning, TEXT("5"));

	switch (ItemType) {
		case EItemType::Weapon:
			if (WeaponManager) {
				WeaponManager->SwapWeapon(Index);
			}
			break;
		
		default:
			break;
	}
}

void AFPSCharacter::OnMovementModeChanged(EMovementMode Prev, uint8 PreviousCustom) {
	Super::OnMovementModeChanged(Prev, PreviousCustom);
	
	if (StateComponent) {
		const bool bFalling = GetMovementComponent()->IsFalling();
		StateComponent->TrySetFalling(bFalling);
	}
}

void AFPSCharacter::UnifiedUIHandler(EButtonType ButtonType, UObject* Context) {
	if (!UIManager) return;

	switch (ButtonType) {
	case EButtonType::Drop:
		if (IUISlotIndexReceiver* SlotReceiver = Cast<IUISlotIndexReceiver>(Context)) {
			Inventory->DropItem(SlotReceiver->GetSlotIndex());
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

void AFPSCharacter::UpdateIsCloseToWall() const {
	FVector Origin = Camera->GetComponentLocation();
	FVector Direction = Camera->GetForwardVector();
	FVector End = Origin + Direction * DistanceToWall;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceTestByChannel(Origin, End, ECC_Visibility, Params)) {
		if (StateComponent) StateComponent->TrySetCloseToWall(true);
	}
	else {
		if (StateComponent) StateComponent->TrySetCloseToWall(false);
	}
}

FWeaponIKData AFPSCharacter::GetWeaponIKData() {
	if (!WeaponManager || !WeaponManager.Get()->GetCurrentWeapon()) {
		FWeaponIKData EmptyData{};

		EmptyData.IKAlpha = 0.f;
		EmptyData.HasGrip = false;

		return EmptyData;
	}

	return WeaponManager.Get()->GetCurrentWeapon()->GetWeaponIKData();
}

void AFPSCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const {
	if (Camera) {
		OutLocation = Camera->GetComponentLocation();
		OutRotation = Camera->GetForwardVector().Rotation();
	}
}

void AFPSCharacter::HandleSlotCleared(int32 SlotIndex) {
	if (!Inventory || !WeaponManager) return;

	if (SlotIndex == WeaponManager->GetCurrentWeaponIndex()) {
		WeaponManager->ImmediateUnEquip();
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

void AFPSCharacter::SetCameraRelativeLocation(FVector NewLocation) {
	Camera->SetRelativeLocation(NewLocation);
}

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

void AFPSCharacter::UpdateMouseInputThisFrame() {
	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
		float DeltaX = 0.f;
		float DeltaY = 0.f;
		PC->GetInputMouseDelta(DeltaX, DeltaY);

		MouseInputThisFrame = FVector2D(DeltaX, DeltaY);
	}
}

void AFPSCharacter::UpdateMovementState() {
	float CurrentVelocity = GetVelocity().SizeSquared2D();

	if (StateComponent) {
		if (CurrentVelocity > FMath::Square(670.f)) {
			StateComponent->TrySetMovementState(EMovementState::Sprinting);
		}
		else if (CurrentVelocity > FMath::Square(50.f)) {
			StateComponent->TrySetMovementState(EMovementState::Walking);
		}
		else {
			StateComponent->TrySetMovementState(EMovementState::Idle);
		}
	}
}

void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

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

	if (Camera) {
		Camera->AttachToComponent(
			CharacterArms.Get(),
			FAttachmentTransformRules::KeepRelativeTransform,
			CameraSocket
		);
	}

	if (WeaponManager) {
		WeaponManager->OnWeaponChanged.AddUObject(
			ViewModel.Get(),
			&UViewModelComponent::OnWeaponChanged
		);

		WeaponManager->OnWeaponMontageStarted.AddUObject(
			ViewModel.Get(),
			&UViewModelComponent::HandleWeaponMontageEvent
		);
	}
	
	if (ViewModel) {
		ViewModel->OnMontageFinished.AddUObject(
			WeaponManager.Get(),
			&UWeaponManager::HandleMontageFinished
		);
	}

	if (Inventory) {
		Inventory->OnInventorySlotCleared.AddUObject(
			this,
			&AFPSCharacter::HandleSlotCleared
		);

		Inventory->OnInventorySlotSelected.AddUObject(
			this,
			&AFPSCharacter::ItemSelectionHandler
		);

		Inventory->OnInventorySlotDropped.AddUObject(
			this,
			&AFPSCharacter::SetDropLocation
		);
	}

	if (Inventory && WeaponManager) {
		Inventory.Get()->OnItemAdded.AddUObject(
			this,
			&AFPSCharacter::ItemSpawnerHandler
		);

		WeaponManager.Get()->OnWeaponLoaded.AddUObject(
			Inventory.Get(),
			&UInventoryComponent::RegisterActorInSlot
		);
	}

	if (ViewModel && WeaponManager) {
		WeaponManager.Get()->OnItemAction.AddUObject(
			ViewModel.Get(),
			&UViewModelComponent::ItemActionHandler
		);
	}

	if (RecoilComponent && WeaponManager) {
		WeaponManager->OnShoot.AddUObject(
			RecoilComponent.Get(),
			&URecoilComponent::AddRecoil
		);
	}

	if (StateComponent) {
		if (FOVComp) {
			StateComponent->OnStatesChanged.AddUObject(
				FOVComp.Get(),
				&UFieldOfViewComponent::PlayerStateHandler
			);
		}
	}
}

void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMovementState();
	UpdateIsCloseToWall();
}

void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

