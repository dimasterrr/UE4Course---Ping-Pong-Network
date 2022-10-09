#include "PingPongPlatform.h"

#include "Components/BoxComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

APingPongPlatform::APingPongPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	MeshComponent->SetIsReplicated(true);

	SetReplicates(true);
	SetReplicateMovement(true);
}

void APingPongPlatform::BeginPlay()
{
	Super::BeginPlay();

	LoadBodyMash();
}

void APingPongPlatform::Server_MoveRight_Implementation(float AxisValue)
{
	const auto CurrentLocation = GetActorLocation();
	const auto NextLocation = CurrentLocation + GetActorRightVector() * MoveSpeed * AxisValue;

	if (!SetActorLocation(NextLocation, true))
	{
		SetActorLocation(CurrentLocation);
	}
}

bool APingPongPlatform::Server_MoveRight_Validate(float AxisValue)
{
	return true;
}

void APingPongPlatform::LoadBodyMash()
{
	FStreamableDelegate LoadDelegate;
	LoadDelegate.BindUObject(this, &ThisClass::OnAsyncAssetLoaded);

	AssetHandler = UAssetManager::GetStreamableManager().RequestAsyncLoad(BodyMeshRef.ToSoftObjectPath(), LoadDelegate);
}

void APingPongPlatform::OnAsyncAssetLoaded()
{
	if (const auto BodyMesh = Cast<UStaticMesh>(AssetHandler.Get()->GetLoadedAsset()))
	{
		MeshComponent->SetStaticMesh(Cast<UStaticMesh>(BodyMesh));
	}
}

void APingPongPlatform::SetSkin_Implementation(UMaterialInterface* NewSkin)
{
	if (!MeshComponent) return;

	MeshComponent->SetMaterial(0, NewSkin);
}
