#include "GameObjectAsset.h"
#include "Engine/PrimaryAssetLabel.h"

FString FGameObjectInfo::GetGameObjectReference()
{
	if (!PrimaryAssetLabel)
	{
		PrimaryAssetLabel = Cast<UPrimaryAssetLabel>(FSoftObjectPath(PrimaryAsset).TryLoad());
	}

	if (PrimaryAssetLabel && PrimaryAssetLabel->ExplicitAssets.Num() > 0)
	{
		return PrimaryAssetLabel->ExplicitAssets[0].GetLongPackageName();
	}

	const auto AssetPath = PrimaryAsset.GetAssetPathString();
	return AssetPath.Right(AssetPath.Len() - AssetPath.Find(".") - 1);
}
