// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LousyMemoryFunctionLibrary.generated.h"


/*Enum Declaration For ConvertMemoryUnit Conversion */
UENUM(BlueprintType)
enum class ELousyMemoryUnit : uint8
{
    KB UMETA(DisplayName = "KB"),
    MB UMETA(DisplayName = "MB"),
    GB UMETA(DisplayName = "GB")
};

UCLASS()
class LOUSYMEMORYMANAGER_API ULousyMemoryFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    /** Force unloads a batch of soft object references immediately */
    UFUNCTION(BlueprintCallable, Category = "Lousy Memory Manager", meta = (ToolTip = "Forces unloading of a batch of soft-referenced assets to free memory."))
    static void ForceUnloadAssetBatch(const TArray<TSoftObjectPtr<UObject>>& AssetsToUnload);

    /** Force unloads a single soft object reference immediately */
    UFUNCTION(BlueprintCallable, Category = "Lousy Memory Manager", meta = (ToolTip = "Forces unloading of the specified soft-referenced asset to free memory."))
    static void ForceUnloadAsset(const TSoftObjectPtr<UObject> AssetToUnload);

    /** Force Garbage Collection immediately, with optional full purge */
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", ToolTip = "Forces immediate garbage collection in the provided world context. Enable 'Full Purge' to purge all unused objects."), Category = "Lousy Memory Manager")
    static void ForceGarbageCollection(UObject* WorldContextObject, bool bFullPurge = false);

    /** Starts scheduled garbage collection at specified interval */
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", ToolTip = "Starts scheduled garbage collection in the provided world context at defined intervals. Set 'Full Purge' to true for a complete memory purge."), Category = "Lousy Memory Manager")
    static void StartScheduledGarbageCollection(UObject* WorldContextObject, float IntervalSeconds, bool bFullPurge = false);

    /** Stops scheduled garbage collection */

    UFUNCTION(BlueprintCallable, Category = "Lousy Memory Manager", meta = (WorldContext = "WorldContextObject", ToolTip = "Stops the scheduled garbage collection process in the provided world context."))
    static void StopScheduledGarbageCollection(UObject* WorldContextObject);

    /** Checks if the soft object reference is currently loaded in memory */
    UFUNCTION(BlueprintPure, Category = "Lousy Memory Manager", meta = (ToolTip = "Checks whether the specified asset is currently loaded in memory. Returns true if loaded, false otherwise."))
    static bool IsAssetLoadedInMemory(const TSoftObjectPtr<UObject>& AssetRef);

    /** Returns the memory footprint of the loaded asset (in KB, float). Returns 0.0 if not loaded */
    UFUNCTION(BlueprintPure, Category = "Lousy Memory Manager", meta = (ToolTip = "Returns the memory footprint of the specified asset reference in Megabytes. Useful for analyzing individual asset memory usage."))
    static float GetAssetMemoryFootprint(const TSoftObjectPtr<UObject>& AssetRef);

    /*This functions takes in the memory size and unit and returns the memory size in KB, MB and GB*/
    UFUNCTION(BlueprintPure, Category = "Lousy Memory Manager", meta = (ToolTip = "Converts the given memory size value into Kilobytes, Megabytes, and Gigabytes based on the selected unit."))
    static void ConvertMemoryUnits(float SizeValue, ELousyMemoryUnit Unit, float& SizeInKB, float& SizeInMB, float& SizeInGB);

    /*Nullifies Hard object refrences for the input array and Returns total number of refrences nullified*/
    UFUNCTION(BlueprintCallable, Category = "Lousy Memory Manager", meta = (DisplayName = "Nullify Hard References(Experimental)", ToolTip = "Attempts to nullify hard references of a given class type within the asset array. Includes a dry run option to simulate changes without applying. Experimental feature, use cautiously."))
    static int32 NullifyHardReferences(UClass* ClassType, const TArray<TSoftObjectPtr<UObject>>& AssetArray, bool bDryRun);

    /** Returns the memory footprint of an array of loaded asset (in KB, float). Returns 0.0 if not loaded */
    UFUNCTION(BlueprintPure, Category = "Lousy Memory Manager", meta = (ToolTip = "Returns the total memory footprint (in KB) of the provided array of soft object references."))
    static float GetMultiAssetMemoryFootprint(const TArray<TSoftObjectPtr<UObject>>& AssetArray);

    /*Force Unloads all objects of given class*/
    UFUNCTION(BlueprintCallable, Category = "Lousy Memory Manager", meta = (DisplayName = "Force Unload All Assets Of Class", ToolTip = "Forces unloading of all loaded assets of the specified class to free up memory."))
    static void ForceUnloadAllAssetsOfClass(UClass* ClassFilter);

    /*Gets all loaded assets of a certain class*/
    UFUNCTION(BlueprintCallable, Category = "Lousy Memory Manager", meta = (DisplayName = "Get Loaded Assets Of Class", ToolTip = "Returns all loaded assets of the specified class. Helps monitor memory usage."))
    static TArray<UObject*> GetLoadedAssetsOfClass(UClass* ClassFilter);

    /*Clears Soft object paths, not meant to be used for reusable assets only for assets that wont be refrenced again*/
    UFUNCTION(BlueprintCallable, Category = "Lousy Memory Manager", meta = (DisplayName = "Kill Soft References (Experimental)", ToolTip = "EXPERIMENTAL: Clears soft references. Not recommended for production builds.", Keywords = "Experimental Memory Unsafe"))
    static void KillSoftRefrences(UPARAM(ref) TArray<TSoftObjectPtr<UObject>>& AssetArray);
};
