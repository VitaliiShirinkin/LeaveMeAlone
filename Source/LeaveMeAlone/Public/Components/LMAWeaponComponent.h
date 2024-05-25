// LeaveMeAlone Game by Netologiya. All RightsReserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LMAWeaponComponent.generated.h"

class ALMABaseWeapon;
class FTimerManager;
struct FAmmoWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEAVEMEALONE_API ULMAWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULMAWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<ALMABaseWeapon> WeaponClass;

    UPROPERTY()
    ALMABaseWeapon* Weapon = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    UAnimMontage* ReloadMontage;

    bool AnimReloading = false;

    FTimerHandle FireTimerHandle;

public:
    UFUNCTION(BlueprintCallable)
    bool GetCurrentWeaponAmmo(FAmmoWeapon& AmmoWeapon) const;

	void SpawnWeapon();
    void Fire();

    UFUNCTION(BlueprintCallable)
    void StopFire();

    void Reload();
    void ReloadProcess();
    void InitAnimNotify();
    void OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh);
    bool CanReload() const;
};
