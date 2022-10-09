#include "DLCLoader.h"

#include "GameObjectAsset.h"
#include "IPlatformFilePak.h"
#include "Engine/AssetManager.h"

bool FDLCLoaderFileVisitor::Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory)
{
	if (!bIsDirectory) Files.Add(FilenameOrDirectory);

	return true;
}

TArray<FString> UDLCLoader::GetClassesToLoad()
{
	TArray<FString> OutClasses;
	TArray<FName> BaseClasses;
	TSet<FName> ExcludedClasses;
	TSet<FName> DerivedClasses;

	BaseClasses.Add(UGameObjectAsset::StaticClass()->GetFName());
	const auto AssetManager = &UAssetManager::Get();

	AssetManager->GetAssetRegistry().GetDerivedClassNames(BaseClasses, ExcludedClasses, DerivedClasses);

	for (FName Derived : DerivedClasses)
	{
		auto DerivedString = Derived.ToString();

		if (DerivedString.EndsWith("_C"))
			DerivedString = DerivedString.Mid(0, DerivedString.Len() - 2);

		OutClasses.Add(*DerivedString);
	}

	return OutClasses;
}

TArray<FString> UDLCLoader::LoadDLC()
{
	TArray<FString> LoadedPlugins;

#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("Run in Editor mode. DLC loadingaborted"));
	return LoadedPlugins;
#endif

	const auto PathToDLCFolder = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "DLC";
	UE_LOG(LogTemp, Warning, TEXT("DLC PATH: %s"), *PathToDLCFolder);

	const auto FileManager = &IFileManager::Get();
	if (!FPaths::DirectoryExists(PathToDLCFolder))
	{
		UE_LOG(LogTemp, Warning, TEXT("DLC Directory %s not found"), *PathToDLCFolder);
		FileManager->MakeDirectory(*PathToDLCFolder, true);
		return LoadedPlugins;
	}

	TArray<FString> PakFiles;
	const FString DLCExtension = "*.pak";
	FileManager->FindFilesRecursive(PakFiles, *PathToDLCFolder, *DLCExtension, true, false);
	for (auto i = 0; i < PakFiles.Num(); ++i)
	{
		MountDLC(PakFiles[i]);
		ReadPakFile(PakFiles[i]);
	}
	return LoadedPlugins;
}

bool UDLCLoader::MountDLC(const FString& PakFilename)
{
	const auto PakOrder = GetDLCOrder(PakFilename);
	const auto DLCPakFile = GetDLCFile();

	TArray<FString> MountedPoints1;
	DLCPakFile->GetMountedPakFilenames(MountedPoints1);

	const bool Result = DLCPakFile->Mount(*PakFilename, PakOrder, nullptr);

	TArray<FString> MountedPoints2;
	DLCPakFile->GetMountedPakFilenames(MountedPoints2);

	return Result;
}

bool UDLCLoader::UnmountDLC(const FString& PakFilename)
{
	const auto DLCPakFile = GetDLCFile();

	TArray<FString> MountedPoints1;
	DLCPakFile->GetMountedPakFilenames(MountedPoints1);

	if (MountedPoints1.Contains(PakFilename))
	{
		UE_LOG(LogTemp, Warning, TEXT("Mount point %s exists! Unmount started..."), *PakFilename);
		if (DLCPakFile->Unmount(*PakFilename))
		{
			UE_LOG(LogTemp, Warning, TEXT("Unmounted!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Unmounting error!"));
			return false;
		}
	}

	return true;
}

int32 UDLCLoader::GetDLCOrder(const FString& PakFilePath)
{
	if (PakFilePath.StartsWith(FString::Printf(TEXT("%sPaks/%s-"),
	                                           *FPaths::ProjectContentDir(), FApp::GetProjectName())))
	{
		return 4;
	}

	if (PakFilePath.StartsWith(FPaths::ProjectContentDir()))
	{
		return 3;
	}

	if (PakFilePath.StartsWith(FPaths::EngineContentDir()))
	{
		return 2;
	}

	if (PakFilePath.StartsWith(FPaths::ProjectSavedDir()))
	{
		return 1;
	}

	return 0;
}

void UDLCLoader::RegisterMountPoint(const FString& RootPath, const FString& ContentPath)
{
	FPackageName::RegisterMountPoint(RootPath, ContentPath);
}

TArray<FString> UDLCLoader::GetFilesInDLC(const FString& Directory)
{
	FDLCLoaderFileVisitor Visitor;
	GetDLCFile()->IterateDirectory(*Directory, Visitor);

	return Visitor.Files;
}

bool UDLCLoader::ReadPakFile(FString PakFileName)
{
	UE_LOG(LogTemp, Warning, TEXT("ReadPakFile: Mount pak file : %s"), *PakFileName);
	FPakPlatformFile* PakPlatformFile;

	{
		if (const FString PlatformFileName = FPlatformFileManager::Get().GetPlatformFile().GetName();
			PlatformFileName.Equals(FString(TEXT("PakFile"))))
		{
			PakPlatformFile = static_cast<FPakPlatformFile*>(&FPlatformFileManager::Get().GetPlatformFile());
		}
		else
		{
			PakPlatformFile = new FPakPlatformFile;
			if (!PakPlatformFile->Initialize(&FPlatformFileManager::Get().GetPlatformFile(), TEXT("")))
			{
				UE_LOG(LogTemp, Error, TEXT("FPakPlatformFile failed to initialize"));
				return false;
			}

			FPlatformFileManager::Get().SetPlatformFile(*PakPlatformFile);
		}
	}

	auto PakFilePathFull = FPaths::ConvertRelativePathToFull(PakFileName);
	auto PakFile = new FPakFile(PakPlatformFile, *PakFilePathFull, false);

	TArray<FString> FileList;

	auto PackName = FPaths::GetBaseFilename(PakFileName);
	auto MountPoint = PakFile->GetMountPoint();
	PakFile->FindPrunedFilesAtPath(FileList, *MountPoint, true, false, true);

	FStreamableManager StreamableManager;
	for (auto k = 0; k < FileList.Num(); ++k)
	{
		if (auto AssetName = FileList[k]; AssetName.Contains("BP_") && AssetName.Contains(".uasset"))
		{
			UE_LOG(LogTemp, Log, TEXT("Loading Pak: %s from File %s..."), *PackName, *FileList[k]);

			auto AssetShortName = FPackageName::GetShortName(AssetName);

			FString FileName, FileExt;
			AssetShortName.Split(TEXT("."), &FileName, &FileExt);

			FString NewAssetName = FString("/") + PackName + "/" + FileName + TEXT(".") + FileName + "_C";
			UE_LOG(LogTemp, Log, TEXT("Loading Asset %s ..."), *NewAssetName);

			UObject* ObjectToLoad = nullptr;
			auto Ref = FSoftObjectPath(NewAssetName);

			UE_LOG(LogTemp, Warning, TEXT("load object..."));
			ObjectToLoad = Ref.TryLoad();

			if (ObjectToLoad)
			{
				auto GeneratedClass = Cast<UBlueprintGeneratedClass>(ObjectToLoad);

				if (auto GameObjectAsset = Cast<UGameObjectAsset>(GeneratedClass->GetDefaultObject()))
				{
					GameObjectsInfo.Add(GameObjectAsset->GameObjectInfo);
					UE_LOG(LogTemp, Warning, TEXT("Add data %s"), *GameObjectAsset->GetFullName());
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("File %s loading error!"), *FileList[k]);
				}
			}
		}
	}
	return true;
}

UClass* UDLCLoader::LoadClassFromDLC(const FString& Filename)
{
	const auto Name = Filename + TEXT(".") + FPackageName::GetShortName(Filename) + TEXT("_C");
	return StaticLoadClass(UObject::StaticClass(), nullptr, *Name);
}

FPakPlatformFile* UDLCLoader::GetDLCFile()
{
	if (!DLCFile)
	{
		if (const auto CurrentPlatformFile = FPlatformFileManager::Get().FindPlatformFile(TEXT("PakFile")))
		{
			DLCFile = static_cast<FPakPlatformFile*>(CurrentPlatformFile);
		}
		else
		{
			DLCFile = new FPakPlatformFile();
			ensure(DLCFile != nullptr);

			auto& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

#if UE_BUILD_SHIPPING == 0
			OriginalPlatformFile = &PlatformFile;
#endif

			if (DLCFile->Initialize(&PlatformFile, TEXT("")))
			{
				FPlatformFileManager::Get().SetPlatformFile(*DLCFile);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("DLCFile initialization error!"));
			}
		}
	}

	ensure(DLCFile != nullptr);
	return DLCFile;
}

bool UDLCLoader::ReadDLCGameObjectsInfo()
{
#if UE_BUILD_SHIPPING
	//
#else
	const auto AssetManager = &UAssetManager::Get();
	auto ClassesToLoad = GetClassesToLoad();

	if (TArray<FAssetData> RegistryData; AssetManager->GetAssetRegistry().GetAllAssets(RegistryData))
	{
		for (FAssetData& Data : RegistryData)
		{
			for (FString ClassToLoad : ClassesToLoad)
			{
				if (Data.AssetName == FName(*ClassToLoad))
				{
					UObject* ObjectToLoad = nullptr;
					if (auto GeneratedClassPath = Data.TagsAndValues.FindTag(FName("GeneratedClass"));
						GeneratedClassPath.IsSet())
					{
						const FString ClassObjectPath = FPackageName::ExportTextPathToObjectPath(
							*GeneratedClassPath.GetValue());
						const FString ClassName = FPackageName::ObjectPathToObjectName(ClassObjectPath);
						ObjectToLoad = FSoftObjectPath(ClassObjectPath).TryLoad();
					}

					if (ObjectToLoad)
					{
						const auto GeneratedClass = Cast<UBlueprintGeneratedClass>(ObjectToLoad);
						if (const auto GameObjectAsset = Cast<UGameObjectAsset>(GeneratedClass->GetDefaultObject()))
						{
							GameObjectsInfo.Add(GameObjectAsset->GameObjectInfo);
						}
					}
				}
			}
		}
	}
#endif

	LoadDLC();

	return GameObjectsInfo.Num() > 0;
}

TArray<FGameObjectInfo> UDLCLoader::GetGameObjectsInfo()
{
	if (GameObjectsInfo.Num() == 0)
		ReadDLCGameObjectsInfo();

	return GameObjectsInfo;
}

void UDLCLoader::Clear()
{
	GameObjectsInfo.Empty();
}
