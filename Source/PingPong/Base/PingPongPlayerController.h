#pragma once

#include "CoreMinimal.h"
#include "PingPongPlayerController.generated.h"

class UPingPongPlayerWidget;
class APingPongPlatform;
class APingPongGate;

UCLASS()
class PINGPONG_API APingPongPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	FTransform StartPointTransform;

	UPROPERTY(Replicated)
	int32 PlayerId = INDEX_NONE;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<APingPongPlatform> PlatformClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	APingPongPlatform* PlatformReference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	APingPongGate* GateReference;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UPingPongPlayerWidget> WidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UPingPongPlayerWidget* WidgetReference;

protected:
	UFUNCTION()
	void MoveRight(float AxisValue);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float AxisValue);

public:
	APingPongPlayerController();

	virtual void SetupInputComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetStartTransform(const FTransform& Transform);

	UFUNCTION(BlueprintCallable)
	void SetPlatformSkin(FSoftObjectPath SkinAsset);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Initialize(int32 NewPlayerId, APingPongGate* NewPlayerGate);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Finalize();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SpawnPlatform(TSubclassOf<APingPongPlatform> Class);

	UFUNCTION(Server, Reliable)
	void Server_SetPlatformSkin(FSoftObjectPath SkinAsset);

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_InitializeHud();

	UFUNCTION(Client, Reliable)
	void Client_UpdateWidgetScore(int32 PlayerScore, int32 EnemyScore);

	UFUNCTION(Client, Reliable)
	void Client_UpdateWidgetWaitScreen(bool Enable);

	int32 GetPlayerId() const;
};
