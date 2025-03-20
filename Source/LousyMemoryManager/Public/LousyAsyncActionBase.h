// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Engine/StreamableManager.h"
#include "LousyAsyncActionBase.generated.h"

// Delegates (No parameters)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadFailed);

UCLASS(meta = (HideThen = "true"))
class LOUSYMEMORYMANAGER_API ULousyAsyncActionBase : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", DisplayName = "Async Load Asset Batch", ToolTip = "Asynchronously loads a batch of soft-referenced assets to avoid blocking the game thread."), Category = "Lousy Memory Manager")
    static ULousyAsyncActionBase* LoadAssetsAsync(UObject* WorldContextObject, const TArray<TSoftObjectPtr<UObject>>& AssetsToLoad);

    // Exec output pins
    UPROPERTY(BlueprintAssignable)
    FOnLoadStarted OnStarted;

    UPROPERTY(BlueprintAssignable)
    FOnLoadCompleted OnCompleted;

    UPROPERTY(BlueprintAssignable)
    FOnLoadFailed OnFailed;

protected:

    virtual void Activate() override;

private:

    UObject* WorldContext;
    TArray<TSoftObjectPtr<UObject>> AssetsToLoad;

    TSharedPtr<FStreamableManager> StreamableManager;
    TArray<FSoftObjectPath> ValidLoadedAssetPaths;

    int32 PendingLoadCount = 0;

    void HandleSingleAssetLoaded();
};
