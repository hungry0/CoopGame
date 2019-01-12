// Fill out your copyright notice in the Description page of Project Settings.

#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Engine/EngineTypes.h"
#include "CoopGame.h"


static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
    TEXT("COOP.DebugWeaponDrawing"),
    DebugWeaponDrawing,
    TEXT("Draw Debug Lines for weapons"),
    ECVF_Cheat
);

// Sets default values
ASWeapon::ASWeapon()
{
    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
    SetRootComponent(MeshComp);

    MuzzleSocketName = "MuzzleSocket";
    TracerTargetName = "Target";
    BaseDamage = 20.0f;

    RateOfFire = 600.0f;
}

void ASWeapon::BeginPlay()
{
    Super::BeginPlay();

    TimeBetweenShots = 60 / RateOfFire;
}

void ASWeapon::Fire()
{
    AActor* MyOwner = GetOwner();
    if (!MyOwner)
    {
        return;
    }

    FVector EyeLocation;
    FRotator EyeRotation;
    MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

    FVector ShotDirection = EyeRotation.Vector();
    FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(MyOwner);
    QueryParams.AddIgnoredActor(this);
    QueryParams.bTraceComplex = true;
    QueryParams.bReturnPhysicalMaterial = true;

    FVector TracerTargetPoint = TraceEnd;

    FHitResult HitResult;
    if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
    {
        AActor* HitActor = HitResult.GetActor();

        EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

        float ActualDamage = BaseDamage;
        if (SurfaceType == SURFACE_FLESHVAVULNERABLE)
        {
            ActualDamage *= 2.0f;
        }

        UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, HitResult, HitActor->GetInstigatorController(), this, DamageType);

        UParticleSystem* SelectedParticleSystem = nullptr;

        switch (SurfaceType)
        {
        case SURFACE_FLESHDEFAULT:
        case SURFACE_FLESHVAVULNERABLE:
            SelectedParticleSystem = FleshImpactEffect;
            break;
        default:
            SelectedParticleSystem = DefaultImpactEffect;
            break;
        }

        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedParticleSystem, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());

        TracerTargetPoint = HitResult.ImpactPoint;

        LastFireTime = GetWorld()->TimeSeconds;
    }

    if (DebugWeaponDrawing > 0)
    {
        DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 2.0f, 0.0f, 0.5f);
    }

    PlayEffects(TracerTargetPoint);
}

void ASWeapon::StartFire()
{
    float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

    GetWorldTimerManager().SetTimer(TimerHandle_BetweenShots, this, &ASWeapon::Fire, 1.0f, true, FirstDelay);
}

void ASWeapon::StopFire()
{
    GetWorldTimerManager().ClearTimer(TimerHandle_BetweenShots);
}

void ASWeapon::PlayEffects(FVector TracerEnd)
{
    if (MuzzleEffect)
    {
        UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
    }

    FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

    if (TracerEffect)
    {
        UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
        TracerComp->SetVectorParameter(TracerTargetName, TracerEnd);
    }

    APawn* MyOwner = Cast<APawn>(GetOwner());
    if (MyOwner)
    {
        APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
        if (PC)
        {
            PC->ClientPlayCameraShake(FireCamShake);
        }
    }
}


