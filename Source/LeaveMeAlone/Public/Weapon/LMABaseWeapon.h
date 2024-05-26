// LeaveMeAlone Game by Netologiya. All RightsReserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LMABaseWeapon.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNotifyClipEmptySignature);

USTRUCT(BlueprintType)
struct FAmmoWeapon
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    int32 Bullets;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    int32 Clips;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    bool Infinite;
};

class USoundWave;
class UNiagaraSystem;

UCLASS()
class LEAVEMEALONE_API ALMABaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ALMABaseWeapon();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    USkeletalMeshComponent* WeaponComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float TraceDistance = 800.0f;

public:	
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FAmmoWeapon AmmoWeapon{30, 0, true};

    FAmmoWeapon CurrentAmmoWeapon;

    FOnNotifyClipEmptySignature OnNotifyClipEmpty;

    float FireRate = 0.1f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    USoundWave* ShootWave;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UNiagaraSystem* TraceEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FString TraceName = "Tracer";
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float Damage = 20;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
    FAmmoWeapon GetCurrentAmmoWeapon() const { return CurrentAmmoWeapon; }
    void Fire();
    void Shoot();
    void DecrementBullets();
    bool IsCurrentClipEmpty() const;
    bool IsCurrentClipFull() const;
    void ChangeClip();
    void SpawnTrace(const FVector& TraceStart, const FVector& TraceEnd);
    void MakeDamage(const FHitResult& HitResult);
};
