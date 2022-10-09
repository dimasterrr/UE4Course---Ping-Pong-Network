#include "PingPongBall.h"

#include "Components/SphereComponent.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"

APingPongBall::APingPongBall()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetLinearDamping(0.f);
	// MeshComponent->SetConstraintMode(EDOFMode::XYPlane);
	MeshComponent->SetCollisionProfileName("BlockAll");
	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->SetIsReplicated(true);
	MeshComponent->OnComponentHit.AddDynamic(this, &APingPongBall::OnComponentHitEvent);

	SetReplicates(true);
	SetReplicateMovement(true);
}

void APingPongBall::BeginPlay()
{
	Super::BeginPlay();

	AsyncLoadAsset(EAsyncLoaderType::BodyMesh, BodyMeshRef);
	AsyncLoadAsset(EAsyncLoaderType::HitEffect, HitEffectRef);
}

void APingPongBall::OnComponentHitEvent(UPrimitiveComponent* PrimitiveComponent, AActor* Actor,
                                        UPrimitiveComponent* PrimitiveComponent1, FVector Vector,
                                        const FHitResult& HitResult)
{
	Multicast_HitEffect();
}

void APingPongBall::Multicast_HitEffect_Implementation()
{
	if (const auto World = GetWorld(); World && HitEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, HitEffect, GetActorLocation());
	}
}

void APingPongBall::AsyncLoadAsset(const EAsyncLoaderType LoaderType, const TSoftObjectPtr<UObject> Ref)
{
	FStreamableDelegate LoadDelegate;
	LoadDelegate.BindUObject(this, &ThisClass::OnAsyncAssetLoaded, LoaderType);

	const auto AsyncHandler = UAssetManager::GetStreamableManager().RequestAsyncLoad(Ref.ToSoftObjectPath(), LoadDelegate);
	AssetHandles.Add(LoaderType, AsyncHandler);
}

void APingPongBall::OnAsyncAssetLoaded(const EAsyncLoaderType AssetType)
{
	FAsyncHandler Handler;
	AssetHandles.RemoveAndCopyValue(AssetType, Handler);
	
	UObject* LoadedAsset = Handler.Get()->GetLoadedAsset();

	switch (AssetType)
	{
	case EAsyncLoaderType::BodyMesh:
		{
			MeshComponent->SetStaticMesh(Cast<UStaticMesh>(LoadedAsset));
			return;
		}
	case EAsyncLoaderType::HitEffect:
		{
			HitEffect = Cast<UParticleSystem>(LoadedAsset);
			return;
		}
	default: return;
	}
}

// Only for test
void APingPongBall::LoadBodyMesh()
{
	AsyncLoadAsset(EAsyncLoaderType::BodyMesh, BodyMeshRef);
}

void APingPongBall::LoadHitEffect()
{
	AsyncLoadAsset(EAsyncLoaderType::HitEffect, HitEffectRef);
}
