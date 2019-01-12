// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShake;

UCLASS()
class COOPGAME_API ASWeapon : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASWeapon();

protected:
    void PlayEffects(FVector TracerEnd);


    virtual void BeginPlay() override;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* MeshComp;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<UDamageType> DamageType;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName MuzzleSocketName;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName TracerTargetName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* MuzzleEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* DefaultImpactEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* FleshImpactEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* TracerEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<UCameraShake> FireCamShake;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float BaseDamage;

    FTimerHandle TimerHandle_BetweenShots;

    float LastFireTime;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float RateOfFire;

    float TimeBetweenShots;

public:

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void Fire();

    void StartFire();

    void StopFire();
};
