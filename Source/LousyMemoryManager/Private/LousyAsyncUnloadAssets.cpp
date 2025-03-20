// Fill out your copyright notice in the Description page of Project Settings.


#include "LousyAsyncUnloadAssets.h"

ULousyAsyncUnloadAssets* ULousyAsyncUnloadAssets::UnloadAssetsAsync(UObject* WorldContextObject, const TArray<TSoftObjectPtr<UObject>>& AssetsToUnload)
{
    ULousyAsyncUnloadAssets* Node = NewObject<ULousyAsyncUnloadAssets>();
    Node->WorldContext = WorldContextObject;
    Node->AssetsToUnload = AssetsToUnload;
    return Node;
}

void ULousyAsyncUnloadAssets::Activate()
{
    if (AssetsToUnload.Num() == 0)
    {
        OnFailed.Broadcast();
        return;
    }

    if (!StreamableManager.IsValid())
    {
        StreamableManager = MakeShared<FStreamableManager>();
    }

    ValidUnloadAssetPaths.Empty();

    // Validate and collect valid paths
    for (const TSoftObjectPtr<UObject>& Asset : AssetsToUnload)
    {
        FSoftObjectPath AssetPath = Asset.ToSoftObjectPath();

        if (!AssetPath.IsValid())
        {
            // Skip invalid
            continue;
        }

        ValidUnloadAssetPaths.Add(AssetPath);
    }

    if (ValidUnloadAssetPaths.Num() == 0)
    {
        // No valid assets found
        OnFailed.Broadcast();
        return;
    }

    // Fire OnStarted exec pin
    OnStarted.Broadcast();

    // Unload each valid asset individually
    for (const FSoftObjectPath& Path : ValidUnloadAssetPaths)
    {
        StreamableManager->Unload(Path);
    }

    // Fire OnCompleted after all unloaded
    OnCompleted.Broadcast();
}

void ULousyAsyncUnloadAssets::HandleSingleAssetUnloaded()
{
    // Optional placeholder – Unreal's Unload doesn't give per-asset callbacks
}