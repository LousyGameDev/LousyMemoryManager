// Fill out your copyright notice in the Description page of Project Settings.


#include "LousyMemoryFunctionLibrary.h"
#include "Engine/StreamableManager.h"
#include "UObject/GarbageCollection.h"
#include "LousyMemoryManagerSubsystem.h"
#include "Engine/World.h"
#include "Serialization/ArchiveCountMem.h"
#include "UObject/ReferenceChainSearch.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UnrealType.h"
#include "Engine/AssetManager.h"
#include "Engine/GameInstance.h"


void ULousyMemoryFunctionLibrary::ForceUnloadAssetBatch(const TArray<TSoftObjectPtr<UObject>>& AssetsToUnload)
{
    if (AssetsToUnload.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] ForceUnloadAssetBatch: No assets provided."));
        return;
    }

    FStreamableManager StreamableManager;

    for (const TSoftObjectPtr<UObject>& Asset : AssetsToUnload)
    {
        FSoftObjectPath Path = Asset.ToSoftObjectPath();

        if (Path.IsValid())
        {
            StreamableManager.Unload(Path);
            UE_LOG(LogTemp, Log, TEXT("[LousyMemory] Unloaded: %s"), *Path.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] Invalid asset path, skipped."));
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[LousyMemory] ForceUnloadAssetBatch: Completed."));
}

void ULousyMemoryFunctionLibrary::ForceUnloadAsset(const TSoftObjectPtr<UObject> AssetToUnload)
{
    FSoftObjectPath Path = AssetToUnload.ToSoftObjectPath();

    if (!Path.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] ForceUnloadAsset: Invalid asset path, skipped."));
        return;
    }

    FStreamableManager StreamableManager;
    StreamableManager.Unload(Path);

    UE_LOG(LogTemp, Log, TEXT("[LousyMemory] ForceUnloadAsset: Unloaded %s"), *Path.ToString());
}

void ULousyMemoryFunctionLibrary::ForceGarbageCollection(UObject* WorldContextObject, bool bFullPurge)
{
    // Log action
    UE_LOG(LogTemp, Log, TEXT("[LousyMemory] ForceGarbageCollection: Triggered. FullPurge: %s"), bFullPurge ? TEXT("True") : TEXT("False"));

    // Call Unreal's global garbage collection function
    CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS, bFullPurge);
}

void ULousyMemoryFunctionLibrary::StartScheduledGarbageCollection(UObject* WorldContextObject, float IntervalSeconds, bool bFullPurge)
{
    if (!WorldContextObject) return;

    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return;

    if (UGameInstance* GameInstance = World->GetGameInstance())
    {
        ULousyMemoryManagerSubsystem* Subsystem = GameInstance->GetSubsystem<ULousyMemoryManagerSubsystem>();
        if (Subsystem)
        {
            Subsystem->StartScheduledGarbageCollection(IntervalSeconds, bFullPurge);
        }
    }
}

void ULousyMemoryFunctionLibrary::StopScheduledGarbageCollection(UObject* WorldContextObject)
{
    if (!WorldContextObject) return;

    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return;

    if (UGameInstance* GameInstance = World->GetGameInstance())
    {
        ULousyMemoryManagerSubsystem* Subsystem = GameInstance->GetSubsystem<ULousyMemoryManagerSubsystem>();
        if (Subsystem)
        {
            Subsystem->StopScheduledGarbageCollection();
        }
    }
}

bool ULousyMemoryFunctionLibrary::IsAssetLoadedInMemory(const TSoftObjectPtr<UObject>& AssetRef)
{
    return AssetRef.Get() != nullptr;
}

float ULousyMemoryFunctionLibrary::GetAssetMemoryFootprint(const TSoftObjectPtr<UObject>& AssetRef)
{
    UObject* LoadedAsset = AssetRef.Get();

    if (!LoadedAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] GetAssetMemoryFootprintKB: Asset not loaded."));
        return 0.0f;
    }

    FArchiveCountMem MemoryCounter(LoadedAsset);
    int32 Bytes = MemoryCounter.GetNum();
    float KBs = static_cast<float>(Bytes) / 1024.0f;

    UE_LOG(LogTemp, Log, TEXT("[LousyMemory] GetAssetMemoryFootprintKB: Asset %s uses approx %.2f KB"), *LoadedAsset->GetName(), KBs);

    return KBs;
}

void ULousyMemoryFunctionLibrary::ConvertMemoryUnits(float SizeValue, ELousyMemoryUnit Unit, float& SizeInKB, float& SizeInMB, float& SizeInGB)
{
    float SizeInBytes = 0.0f;

    switch (Unit)
    {
    case ELousyMemoryUnit::KB:
        SizeInBytes = SizeValue * 1024.0f;
        break;
    case ELousyMemoryUnit::MB:
        SizeInBytes = SizeValue * 1024.0f * 1024.0f;
        break;
    case ELousyMemoryUnit::GB:
        SizeInBytes = SizeValue * 1024.0f * 1024.0f * 1024.0f;
        break;
    default:
        break;
    }

    SizeInKB = SizeInBytes / 1024.0f;
    SizeInMB = SizeInBytes / (1024.0f * 1024.0f);
    SizeInGB = SizeInBytes / (1024.0f * 1024.0f * 1024.0f);
}

int32 ULousyMemoryFunctionLibrary::NullifyHardReferences(UClass* ClassType, const TArray<TSoftObjectPtr<UObject>>& AssetArray, bool bDryRun)
{
    if (!ClassType || AssetArray.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] NullifyHardReferences: Invalid class type or empty asset array."));
        return 0;
    }

    int32 NullifiedCount = 0;

    for (const TSoftObjectPtr<UObject>& SoftRef : AssetArray)
    {
        if (!SoftRef.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] NullifyHardReferences: Skipping null soft reference."));
            continue;
        }

        UObject* HardRef = SoftRef.Get();
        if (!HardRef || !HardRef->IsA(ClassType))
        {
            continue;
        }

        for (TObjectIterator<UObject> ObjIt; ObjIt; ++ObjIt)
        {
            UObject* Obj = *ObjIt;
            if (!Obj || Obj == HardRef) continue;

            for (TFieldIterator<FProperty> PropIt(Obj->GetClass()); PropIt; ++PropIt)
            {
                FProperty* Property = *PropIt;
                if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Property))
                {
                    void* PropertyValue = ObjProp->ContainerPtrToValuePtr<void>(Obj);
                    if (PropertyValue)
                    {
                        UObject** ObjectProperty = reinterpret_cast<UObject**>(PropertyValue);
                        if (ObjectProperty && *ObjectProperty == HardRef)
                        {
                            if (!bDryRun)
                            {
                                *ObjectProperty = nullptr;
                            }
                            NullifiedCount++;
                        }
                    }
                }
            }
        }
    }

    if (bDryRun)
    {
        UE_LOG(LogTemp, Log, TEXT("[LousyMemory] DryRun: Would nullify %d references."), NullifiedCount);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[LousyMemory] Nullified %d references."), NullifiedCount);
    }

    return NullifiedCount;
}


float ULousyMemoryFunctionLibrary::GetMultiAssetMemoryFootprint(const TArray<TSoftObjectPtr<UObject>>& AssetArray)
{
    float TotalKB = 0.0f;

    for (const TSoftObjectPtr<UObject>& AssetRef : AssetArray)
    {
        UObject* LoadedAsset = AssetRef.Get();

        if (!LoadedAsset)
        {
            continue; // Skip if not loaded
        }

        FArchiveCountMem MemoryCounter(LoadedAsset);
        int32 Bytes = MemoryCounter.GetNum();
        float KB = static_cast<float>(Bytes) / 1024.0f;
        TotalKB += KB;
    }

    UE_LOG(LogTemp, Log, TEXT("[LousyMemory] Total Asset Array Memory Footprint: %.2f KB"), TotalKB);

    return TotalKB;
}


void ULousyMemoryFunctionLibrary::ForceUnloadAllAssetsOfClass(UClass* ClassFilter)
{
    if (!ClassFilter)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] ForceUnloadAllAssetsOfClass: Invalid class."));
        return;
    }

    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

    for (TObjectIterator<UObject> It; It; ++It)
    {
        UObject* Obj = *It;
        if (Obj && Obj->IsA(ClassFilter))
        {
            FSoftObjectPath Path(Obj);
            if (Path.IsValid())
            {
                Streamable.Unload(Path);
                UE_LOG(LogTemp, Log, TEXT("[LousyMemory] Unloaded asset: %s"), *Path.ToString());
            }
        }
    }
}


TArray<UObject*> ULousyMemoryFunctionLibrary::GetLoadedAssetsOfClass(UClass* ClassFilter)
{
    TArray<UObject*> LoadedAssets;

    if (!ClassFilter)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] GetLoadedAssetsOfClass: Invalid class."));
        return LoadedAssets;
    }

    for (TObjectIterator<UObject> It; It; ++It)
    {
        UObject* Obj = *It;
        if (Obj && Obj->IsA(ClassFilter))
        {
            LoadedAssets.Add(Obj);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[LousyMemory] Found %d loaded assets of class."), LoadedAssets.Num());

    return LoadedAssets;
}



void ULousyMemoryFunctionLibrary::KillSoftRefrences(TArray<TSoftObjectPtr<UObject>>& AssetArray)
{
    if (AssetArray.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] ClearSoftReferenceArray: Asset array is empty."));
        return;
    }

    for (TSoftObjectPtr<UObject>& SoftRef : AssetArray)
    {
        SoftRef.Reset(); // Clears both path & reference
    }

    UE_LOG(LogTemp, Log, TEXT("[LousyMemory] Cleared %d soft references."), AssetArray.Num());
}

