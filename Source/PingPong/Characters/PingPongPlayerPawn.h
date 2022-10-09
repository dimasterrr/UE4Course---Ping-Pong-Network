#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PingPongPlayerPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class PINGPONG_API APingPongPlayerPawn : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category=Components)
	USpringArmComponent* SpringArm;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category=Components)
	UCameraComponent* Camera;

public:
	APingPongPlayerPawn();
};
