// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealth.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USHealth*, HealthComp, float, Health, float, HelathDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);


UCLASS( ClassGroup=(Coop), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USHealth : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealth();

    UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "HealthComponent")
    float Health;

    UFUNCTION()
    void OnRep_Health(float OldHealth);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
    float DefaultHealth;

    bool bDead;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
    uint8 TeamNum;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    UFUNCTION()
    void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnHealthChangedSignature OnHealthChanged;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category = "HealthComponent")
    void Heal(float HealAmount);

    float GetHealth() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponent")
    static bool isFriendly(AActor* ActorA, AActor* ActorB);

};
