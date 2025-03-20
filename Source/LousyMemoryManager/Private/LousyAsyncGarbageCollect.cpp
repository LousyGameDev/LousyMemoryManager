// Fill out your copyright notice in the Description page of Project Settings.


#include "LousyAsyncGarbageCollect.h"
#include "Engine/World.h"
#include "UObject/GarbageCollection.h"
#include "TimerManager.h"

ULousyAsyncGarbageCollect* ULousyAsyncGarbageCollect::ForceGarbageCollectionWithDelay(UObject* WorldContextObject, float DelaySeconds, bool bFullPurge)
{
    ULousyAsyncGarbageCollect* Node = NewObject<ULousyAsyncGarbageCollect>();
    Node->WorldContext = WorldContextObject;
    Node->Delay = DelaySeconds;
    Node->bFullPurgeFlag = bFullPurge;
    return Node;
}

void ULousyAsyncGarbageCollect::Activate()
{
    if (!WorldContext)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] GarbageCollect: Invalid WorldContextObject."));
        return;
    }

    UWorld* World = WorldContext->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[LousyMemory] GarbageCollect: Could not get World."));
        return;
    }

    // Fire OnStarted exec pin
    OnStarted.Broadcast();

    // Set timer
    World->GetTimerManager().SetTimer(TimerHandle, [this]()
        {
            UE_LOG(LogTemp, Log, TEXT("[LousyMemory] GarbageCollect: GC Triggered after %.2f seconds. FullPurge: %s"), Delay, bFullPurgeFlag ? TEXT("True") : TEXT("False"));

            CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS, bFullPurgeFlag);

            // Fire OnCompleted exec pin
            OnCompleted.Broadcast();

        }, Delay, false);

    UE_LOG(LogTemp, Log, TEXT("[LousyMemory] GarbageCollect: Delay started (%.2f seconds)."), Delay);
}