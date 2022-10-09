#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingPongBall.generated.h"

struct FStreamableHandle;
class USphereComponent;

UENUM(BlueprintType)
enum class EAsyncLoaderType : uint8
{
	BodyMesh,
	HitEffect
};

UCLASS()
class PINGPONG_API APingPongBall : public AActor
{
	GENERATED_BODY()

private:
	using FAsyncHandler = TSharedPtr<FStreamableHandle>;
	TMap<EAsyncLoaderType, FAsyncHandler> AssetHandles;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category=Components)
	UStaticMeshComponent* MeshComponent;
	
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category=Settings)
    UParticleSystem* HitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Settings)
	TSoftObjectPtr<UStaticMesh> BodyMeshRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Settings)
	TSoftObjectPtr<UParticleSystem> HitEffectRef;
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_HitEffect();

	UFUNCTION(BlueprintCallable, CallInEditor)
	void AsyncLoadAsset(EAsyncLoaderType LoaderType, TSoftObjectPtr<UObject> Ref);

	UFUNCTION()
	void OnAsyncAssetLoaded(EAsyncLoaderType AssetType);

	// Only for editor test
	UFUNCTION(BlueprintCallable, CallInEditor)
	void LoadBodyMesh();
	
	UFUNCTION(BlueprintCallable, CallInEditor)
	void LoadHitEffect();
	
public:
	APingPongBall();
	
	UFUNCTION()
	void OnComponentHitEvent(UPrimitiveComponent* PrimitiveComponent,
	                         AActor* Actor,
	                         UPrimitiveComponent* PrimitiveComponent1,
	                         FVector Vector,
	                         const FHitResult& HitResult);
};
