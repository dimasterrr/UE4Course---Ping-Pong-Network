#pragma once

#include "CoreMinimal.h"
#include "GameObjectAsset.h"
#include "IPlatformFilePak.h"
#include "UObject/Object.h"
#include "DLCLoader.generated.h"

class PINGPONG_API FDLCLoaderFileVisitor : public IPlatformFile::FDirectoryVisitor
{
public:
	TArray<FString> Files;
	
public:
	virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override;
};

UCLASS()
class PINGPONG_API UDLCLoader : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<FGameObjectInfo> GameObjectsInfo;

	FPakPlatformFile* DLCFile = nullptr;

#if UE_BUILD_SHIPPING == 0
	IPlatformFile* OriginalPlatformFile = nullptr;
#endif

private:
	TArray<FString> GetClassesToLoad();

	TArray<FString> LoadDLC();

	// DLC methods
	bool MountDLC(const FString& PakFilename);

	bool UnmountDLC(const FString& PakFilename);

	int32 GetDLCOrder(const FString& PakFilePath);

	void RegisterMountPoint(const FString& RootPath, const FString& ContentPath);

	TArray<FString> GetFilesInDLC(const FString& Directory);

	bool ReadPakFile(FString PakFileName);

	UClass* LoadClassFromDLC(const FString& Filename);

	FPakPlatformFile* GetDLCFile();

public:
	bool ReadDLCGameObjectsInfo();

	TArray<FGameObjectInfo> GetGameObjectsInfo();

	void Clear();
};
