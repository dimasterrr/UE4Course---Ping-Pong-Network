#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PingPongGate.generated.h"

class APlayerStart;
class UBoxComponent;
class USphereComponent;

UCLASS()
class PINGPONG_API APingPongGate : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category=Components)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category=Components)
	UBoxComponent* CollisionComponent;

	UPROPERTY(Replicated)
	int32 PlayerId = INDEX_NONE;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APlayerStart* AttacheToPlayerStart;

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetGoal();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetPlayerId(int32 Value);
	
public:
	APingPongGate();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(BlueprintGetter)
	bool IsReadyToUse() const;

	UFUNCTION(BlueprintCallable)
	void SetGoal();
	
	UFUNCTION()
	void SetPlayerId(int32 Value);
};
