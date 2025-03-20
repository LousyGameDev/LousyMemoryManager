// Fill out your copyright notice in the Description page of Project Settings.


#include "LousyAsyncUnloadSingleAsset.h"

ULousyAsyncUnloadSingleAsset* ULousyAsyncUnloadSingleAsset::UnloadSingleAssetAsync(UObject* WorldContextObject, const TSoftObjectPtr<UObject> AssetToUnload)
{
    ULousyAsyncUnloadSingleAsset* Node = NewObject<ULousyAsyncUnloadSingleAsset>();
    Node->WorldContext = WorldContextObject;
    Node->AssetToUnload = AssetToUnload;
    return Node;
}

void ULousyAsyncUnloadSingleAsset::Activate()
{
    if (!AssetToUnload.IsValid())
    {
        OnFailed.Broadcast();
        return;
    }

    if (!StreamableManager.IsValid())
    {
        StreamableManager = MakeShared<FStreamableManager>();
    }

    FSoftObjectPath AssetPath = AssetToUnload.ToSoftObjectPath();

    if (!AssetPath.IsValid())
    {
        OnFailed.Broadcast();
        return;
    }

    // Fire OnStarted exec pin
    OnStarted.Broadcast();

    // Unload asset
    StreamableManager->Unload(AssetPath);

    // Fire OnCompleted after unloading
    OnCompleted.Broadcast();
}
