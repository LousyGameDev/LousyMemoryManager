// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Engine/StreamableManager.h"
#include "LousyAsyncUnloadSingleAsset.generated.h"


// Unique Delegates for UNLOAD
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnloadSingleStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnloadSingleCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnloadSingleFailed);

UCLASS(meta = (HideThen = "true"))
class LOUSYMEMORYMANAGER_API ULousyAsyncUnloadSingleAsset : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", DisplayName = "Async Unload Single Asset", ToolTip = "Asynchronously unloads a single soft-referenced asset from memory to optimize resource usage without causing frame drops."), Category = "Lousy Memory Manager")
    static ULousyAsyncUnloadSingleAsset* UnloadSingleAssetAsync(UObject* WorldContextObject, const TSoftObjectPtr<UObject> AssetToUnload);


    // Unique Exec output pins
    UPROPERTY(BlueprintAssignable)
    FOnUnloadSingleStarted OnStarted;

    UPROPERTY(BlueprintAssignable)
    FOnUnloadSingleCompleted OnCompleted;

    UPROPERTY(BlueprintAssignable)
    FOnUnloadSingleFailed OnFailed;

protected:

    virtual void Activate() override;

private:

    UObject* WorldContext;
    TSoftObjectPtr<UObject> AssetToUnload;
    TSharedPtr<FStreamableManager> StreamableManager;
};