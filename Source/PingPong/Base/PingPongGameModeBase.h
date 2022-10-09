#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PingPongGameModeBase.generated.h"

class APingPongGate;
class APingPongBall;
class APlayerStart;
class APingPongPlayerController;

UCLASS()
class PINGPONG_API APingPongGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerEntered, APingPongPlayerController*, PlayerController);

protected:
	// Pre Setup
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// int32 MaxPlayerCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APingPongBall> DefaultBallClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Search)
	FName BallStartPointTag = "BallStartPoint";

	// Math Making Fields
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TMap<APingPongPlayerController*, int32> PlayersId;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TMap<int32, int32> PlayersScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APingPongPlayerController* PlayerControllerFirst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APingPongPlayerController* PlayerControllerSecond;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerScoreFirst = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PlayerScoreSecond = 0;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnPlayerEntered OnPlayerEntered;

private:
	void UpdateAvailableGates(TArray<APingPongGate*>& AvailableGates);
	void SpawnBall();

protected:
	void OnAllPlayerIsReady();
	
public:
	APingPongGameModeBase();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	UFUNCTION()
	void AppendPlayerGoal(int32 PlayerId);
};
