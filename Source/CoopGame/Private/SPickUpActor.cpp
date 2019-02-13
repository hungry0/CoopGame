// Fill out your copyright notice in the Description page of Project Settings.

#include "SPickUpActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "SPowerupActor.h"


// Sets default values
ASPickUpActor::ASPickUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    SphereComp->SetSphereRadius(75.0f);
    RootComponent = SphereComp;

    DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
    DecalComp->DecalSize = FVector(64, 75, 75);
    DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
    DecalComp->SetupAttachment(RootComponent);

    CooldownDuration = 10.0f;

    SetReplicates(true);
}

// Called when the game starts or when spawned
void ASPickUpActor::BeginPlay()
{
	Super::BeginPlay();

    if (Role == ROLE_Authority)
    {
        Respawn();
    }
}

// Called every frame
void ASPickUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASPickUpActor::Respawn()
{
    if (PowerupClass == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Powerup class is null in %s, please update your blueprint."), *GetName());

        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    PowerupInstance = GetWorld()->SpawnActor<ASPowerupActor>(PowerupClass, GetTransform(), SpawnParams);
}

void ASPickUpActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (PowerupInstance && Role == ROLE_Authority)
    {
        PowerupInstance->ActivatePowerup();

        PowerupInstance = nullptr;
    }

    GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPickUpActor::Respawn, CooldownDuration);
}

