#pragma once

#include "CoreMinimal.h"
#include "GameObjectAsset.h"
#include "PakGameInstance.generated.h"

UCLASS()
class PINGPONG_API UPakGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	class UDLCLoader* DLCLoader;

protected:
	virtual void Init() override;

public:
	UFUNCTION(BlueprintCallable)
	TArray<FGameObjectInfo> GetGameObjectsInfo() const;
};
