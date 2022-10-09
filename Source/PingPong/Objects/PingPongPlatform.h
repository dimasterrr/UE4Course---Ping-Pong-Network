#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingPongPlatform.generated.h"

struct FStreamableHandle;
class UBoxComponent;

UCLASS()
class PINGPONG_API APingPongPlatform : public AActor
{
	GENERATED_BODY()

private:
	using FAsyncHandler = TSharedPtr<FStreamableHandle>;
	FAsyncHandler AssetHandler;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category=Components)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Settings)
	TSoftObjectPtr<UStaticMesh> BodyMeshRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveSpeed = 100.f;

protected:
	void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, CallInEditor)
	void LoadBodyMash();

	UFUNCTION()
	void OnAsyncAssetLoaded();

public:
	APingPongPlatform();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float AxisValue);

	UFUNCTION(NetMulticast, Reliable)
	void SetSkin(UMaterialInterface* NewSkin);
};
