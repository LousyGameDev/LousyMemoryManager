// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Engine/StreamableManager.h"
#include "LousyAsyncUnloadAssets.generated.h"

// Delegates (No parameters for simplicity)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnloadStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnloadCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnloadFailed);

UCLASS(meta = (HideThen = "true"))
class LOUSYMEMORYMANAGER_API ULousyAsyncUnloadAssets : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:

    // Factory function to spawn the node in Blueprints
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", DisplayName = "Async Unload Asset Batch", ToolTip = "Asynchronously unloads a batch of soft-referenced assets from memory, freeing resources without interrupting gameplay."), Category = "Lousy Memory Manager")
    static ULousyAsyncUnloadAssets* UnloadAssetsAsync(UObject* WorldContextObject, const TArray<TSoftObjectPtr<UObject>>& AssetsToUnload);

    // Exec output pins
    UPROPERTY(BlueprintAssignable)
    FOnUnloadStarted OnStarted;

    UPROPERTY(BlueprintAssignable)
    FOnUnloadCompleted OnCompleted;

    UPROPERTY(BlueprintAssignable)
    FOnUnloadFailed OnFailed;

protected:

    virtual void Activate() override;

private:

    UObject* WorldContext;
    TArray<TSoftObjectPtr<UObject>> AssetsToUnload;

    TSharedPtr<FStreamableManager> StreamableManager;
    TArray<FSoftObjectPath> ValidUnloadAssetPaths;

    int32 PendingUnloadCount = 0;

    void HandleSingleAssetUnloaded();
};