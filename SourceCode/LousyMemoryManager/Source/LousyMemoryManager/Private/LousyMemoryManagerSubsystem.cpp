// Fill out your copyright notice in the Description page of Project Settings.


#include "LousyMemoryManagerSubsystem.h"
#include "UObject/GarbageCollection.h"
#include "Engine/World.h"
#include "TimerManager.h"

void ULousyMemoryManagerSubsystem::StartScheduledGarbageCollection(float IntervalSeconds, bool bFullPurge)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] Scheduled GC: Failed to get World."));
        return;
    }

    bScheduledFullPurge = bFullPurge;

    // Clear any existing timer
    World->GetTimerManager().ClearTimer(ScheduledGCTimerHandle);

    // Set the timer
    World->GetTimerManager().SetTimer(ScheduledGCTimerHandle, [this]()
        {
            UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] Scheduled Garbage Collection Triggered! FullPurge: %s"),
                bScheduledFullPurge ? TEXT("True") : TEXT("False"));

            CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS, bScheduledFullPurge);

        }, IntervalSeconds, true); // true = looping

    UE_LOG(LogTemp, Log, TEXT("[LousyMemory] Scheduled GC Started: Every %.2f seconds. FullPurge: %s"),
        IntervalSeconds,
        bFullPurge ? TEXT("True") : TEXT("False"));
}


void ULousyMemoryManagerSubsystem::StopScheduledGarbageCollection()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] Scheduled GC: Failed to get World."));
        return;
    }

    World->GetTimerManager().ClearTimer(ScheduledGCTimerHandle);


    UE_LOG(LogTemp, Log, TEXT("[LousyMemory] Scheduled Garbage Collection Stopped."));
}