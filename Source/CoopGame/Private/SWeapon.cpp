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
#include "UnrealNetwork.h"


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

    SetReplicates(true);

    MinNetUpdateFrequency = 33.0f;
    NetUpdateFrequency = 66.0f;

    BulletSpherd = 1.0f;
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
}

void ASWeapon::BeginPlay()
{
    Super::BeginPlay();

    TimeBetweenShots = 60 / RateOfFire;
}

void ASWeapon::Fire()
{
    if (Role < ROLE_Authority)
    {
        ServerFire();
    }

    AActor* MyOwner = GetOwner();
    if (!MyOwner)
    {
        return;
    }

    FVector EyeLocation;
    FRotator EyeRotation;
    MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

    FVector ShotDirection = EyeRotation.Vector();

    float HalfRad = FMath::DegreesToRadians(BulletSpread);
    ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);


    FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(MyOwner);
    QueryParams.AddIgnoredActor(this);
    QueryParams.bTraceComplex = true;
    QueryParams.bReturnPhysicalMaterial = true;

    FVector TracerTargetPoint = TraceEnd;
    EPhysicalSurface SurfaceType = SurfaceType_Default;

    FHitResult HitResult;
    if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
    {
        AActor* HitActor = HitResult.GetActor();

        SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

        float ActualDamage = BaseDamage;
        if (SurfaceType == SURFACE_FLESHVAVULNERABLE)
        {
            ActualDamage *= 2.0f;
        }

        UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, HitResult, GetOwner()->GetInstigatorController(), this, DamageType);

        PlayImpactEffects(SurfaceType, HitResult.ImpactPoint);

        TracerTargetPoint = HitResult.ImpactPoint;

        LastFireTime = GetWorld()->TimeSeconds;
    }

    if (DebugWeaponDrawing > 0)
    {
        DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 2.0f, 0.0f, 0.5f);
    }

    PlayEffects(TracerTargetPoint);

    if (Role == ROLE_Authority)
    {
        HitScanTrace.TraceTo = TracerTargetPoint;
        HitScanTrace.SurfaceType = SurfaceType;
    }
}

void ASWeapon::ServerFire_Implementation()
{
    Fire();
}

bool ASWeapon::ServerFire_Validate()
{
    return true;
}

void ASWeapon::OnRep_HitScanTrace()
{
    PlayEffects(HitScanTrace.TraceTo);

    PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
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

void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
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

    if (SelectedParticleSystem)
    {
        FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
        FVector ShotDirection = ImpactPoint - MuzzleLocation;
        ShotDirection.Normalize();

        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedParticleSystem, ImpactPoint, ShotDirection.Rotation());
    }
}

