#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PingPongPlayerWidget.generated.h"

class UTextBlock;

UCLASS()
class PINGPONG_API UPingPongPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* PlayerScoreText;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* EnemyScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsWaitScreenEnabled = true;

public:
	UFUNCTION(BlueprintCallable)
	void UpdatePlayerScore(int32 Score);
	
	UFUNCTION(BlueprintCallable)
	void UpdateEnemyScore(int32 Score);

	UFUNCTION(BlueprintCallable)
	void SetWaitScreen(bool Enable);
};
