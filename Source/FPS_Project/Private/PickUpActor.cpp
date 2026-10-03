// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpActor.h"
#include "ItemDefinition.h"
#include "ItemTypes.h"
#include "InventoryComponent.h"

// Sets default values
APickUpActor::APickUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	//Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
}

FExamineItem* APickUpActor::GetExaminationItem() {
	if (!ItemDef) return nullptr;

	return &ItemDef->ExamineData;
}

UPrimitiveComponent* APickUpActor::GetPrimitiveOnInteract() {
	if (!VisualActor) return nullptr;

	UPrimitiveComponent* PhysComp = VisualActor->FindComponentByClass<UPrimitiveComponent>();

	return PhysComp;
}

void APickUpActor::ThrowItem(FVector Direction) {
	if (!VisualActor) return;

	if (UPrimitiveComponent* ActorRootComponent = Cast<UPrimitiveComponent>(VisualActor->GetRootComponent())) {
		ActorRootComponent->AddImpulse(Direction, NAME_None, true);
	}
}

void APickUpActor::SetAmount(int32 NewAmount) {
	Amount = NewAmount;
}

void APickUpActor::SetItemDef(UItemDefinition* NewItemDef) {
	ItemDef = NewItemDef;
}

void APickUpActor::Initialize() {
	if (!IsValid(ItemDef) || ItemDef->WorldActorClass.IsNull()) return;

	FActorSpawnParameters Params;
	Params.Owner = this;

	if (UClass* ActorClass = ItemDef->WorldActorClass.LoadSynchronous()) {
		VisualActor = GetWorld()->SpawnActor<AActor>(ActorClass, GetActorTransform(), Params);

		if (VisualActor) {
			VisualActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

			UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(VisualActor->GetRootComponent());

			RootComponent = Primitive;

			Primitive->SetSimulatePhysics(true);
			Primitive->SetCollisionProfileName(TEXT("PhysicsActor"));
			Primitive->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		}
	}
}

void APickUpActor::Interact_Implementation(AActor* Interactor) {
	if (!ItemDef || !Interactor) return;

	UInventoryComponent* Inv = Interactor->FindComponentByClass<UInventoryComponent>();

	if (Inv && Inv->AddItem(ItemDef, Amount)) {
		VisualActor->Destroy();
		Destroy();
	}
}

// Called when the game starts or when spawned
void APickUpActor::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(ItemDef)) {
		Initialize();
	}
}

// Called every frame
void APickUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

