// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"


// Sets default values
ASPowerupActor::ASPowerupActor()
{
    PowerupInternal = 0.0f;
    TotalNrOfTicks = 0.0f;
}

// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASPowerupActor::OnTickPowerup()
{
    TicksProcessed++;

    OnPowerupTicked();

    if (TicksProcessed > TotalNrOfTicks)
    {
        OnExpired();

        GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
    }
}

void ASPowerupActor::ActivatePowerup()
{
    if (PowerupInternal > 0.0f)
    {
        GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, PowerupInternal, true, 0.0f);
    }
    else
    {
        OnTickPowerup();
    }
}

