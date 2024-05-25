// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "Components/LMAWeaponComponent.h"
#include "Weapon/LMABaseWeapon.h"
#include "GameFramework/Character.h"
#include "Animations/LMAReloadFinishedAnimNotify.h"
#include "TimerManager.h"

// Sets default values for this component's properties
ULMAWeaponComponent::ULMAWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void ULMAWeaponComponent::BeginPlay()
{
	Super::BeginPlay();	

    SpawnWeapon();
    InitAnimNotify();
}

bool ULMAWeaponComponent::GetCurrentWeaponAmmo(FAmmoWeapon& AmmoWeapon) const
{
    if (IsValid(Weapon))
    {
        AmmoWeapon = Weapon->GetCurrentAmmoWeapon();
        return true;
    }
    return false;
}

void ULMAWeaponComponent::SpawnWeapon()
{
    Weapon = GetWorld()->SpawnActor<ALMABaseWeapon>(WeaponClass);
    if (IsValid(Weapon))
    {
        const auto Character = Cast<ACharacter>(GetOwner());
        if (Character)
        {
            FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
            Weapon->AttachToComponent(Character->GetMesh(), AttachmentRules, "r_Weapon_Socket");
            Weapon->OnNotifyClipEmpty.AddUObject(this, &ULMAWeaponComponent::ReloadProcess);
        }
    }
}

void ULMAWeaponComponent::Fire()
{
    if (IsValid(Weapon) && !AnimReloading)
    {
        Weapon->Fire();
        GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, Weapon, &ALMABaseWeapon::Fire, Weapon->FireRate, true);
    }
}

void ULMAWeaponComponent::StopFire()
{
    GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void ULMAWeaponComponent::Reload()
{
    ReloadProcess();
}

void ULMAWeaponComponent::ReloadProcess()
{
    if (!CanReload()) return;

    Weapon->ChangeClip();
    if (GetWorld()->GetTimerManager().IsTimerActive(FireTimerHandle)) StopFire();
    AnimReloading = true;
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    Character->PlayAnimMontage(ReloadMontage);
}

void ULMAWeaponComponent::InitAnimNotify()
{
    if (!IsValid(ReloadMontage)) return;

    const auto NotifiesEvents = ReloadMontage->Notifies;
    for (auto NotifyEvent : NotifiesEvents)
    {
        auto ReloadFinish = Cast<ULMAReloadFinishedAnimNotify>(NotifyEvent.Notify);
        if (ReloadFinish)
        {
            ReloadFinish->OnNotifyReloadFinished.AddUObject(this, &ULMAWeaponComponent::OnNotifyReloadFinished);
            break;
        }
    }
}

void ULMAWeaponComponent::OnNotifyReloadFinished(USkeletalMeshComponent* SkeletalMesh)
{
    const auto Character = Cast<ACharacter>(GetOwner());
    if (Character->GetMesh() == SkeletalMesh)
    {
        AnimReloading = false;
    }
}

bool ULMAWeaponComponent::CanReload() const
{
    return !AnimReloading && !Weapon->IsCurrentClipFull();
}