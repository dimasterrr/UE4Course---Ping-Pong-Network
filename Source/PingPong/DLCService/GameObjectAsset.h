#pragma once

#include "CoreMinimal.h"
#include "Engine/PrimaryAssetLabel.h"
#include "UObject/Object.h"
#include "GameObjectAsset.generated.h"

UENUM(Blueprintable)
enum class EGameObjectType : uint8
{
	Platform
};

USTRUCT(BlueprintType, Blueprintable)
struct FGameObjectInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EGameObjectType Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Description;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FSoftObjectPath PrimaryAsset;

	UPROPERTY()
	UPrimaryAssetLabel* PrimaryAssetLabel;

	FString GetGameObjectReference();
};

UCLASS()
class PINGPONG_API UObjectInfoBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameObjectInfo | Methods")
	static FString GetGameObjectReference(UPARAM(ref) FGameObjectInfo& MyStructRef)
	{
		return MyStructRef.GetGameObjectReference();
	};
};

UCLASS(BlueprintType, Blueprintable)
class PINGPONG_API UGameObjectAsset : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FGameObjectInfo GameObjectInfo;
};
