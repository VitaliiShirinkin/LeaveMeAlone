// LeaveMeAlone Game by Netologiya. All RightsReserved


#include "Player/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/DamageCameraShake.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/LMAHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/LMAWeaponComponent.h"
#include "GameFramework/HUD.h"

// Sets default values
ALMADefaultCharacter::ALMADefaultCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->SetUsingAbsoluteRotation(true);
    SpringArmComponent->TargetArmLength = ArmLength;
    SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
    SpringArmComponent->bDoCollisionTest = false;
    SpringArmComponent->bEnableCameraLag = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
    CameraComponent->SetupAttachment(SpringArmComponent);
    CameraComponent->SetFieldOfView(FOV);
    CameraComponent->bUsePawnControlRotation = false;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");
    WeaponComponent = CreateDefaultSubobject<ULMAWeaponComponent>("Weapon");
}

// Called when the game starts or when spawned
void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    if (IsValid(CursorMaterial))
    {
        CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
    }

    HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
    OnHealthChanged(HealthComponent->GetHealth());
    HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);
}

// Called every frame
void ALMADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!(HealthComponent->IsDead()) && !IsSprinting)
    {
        RotationPlayerOnCursor();
    }
}

// Called to bind functionality to input
void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);
    PlayerInputComponent->BindAxis("CameraZoom", this, &ALMADefaultCharacter::CameraZoom);
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ALMADefaultCharacter::OnSprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ALMADefaultCharacter::SprintOff);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Fire);
    PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &ULMAWeaponComponent::StopFire);
    PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Reload);
}

void ALMADefaultCharacter::MoveForward(float Value)
{
    if (!IsSprinting)
    {
        AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
    }
    else
    {
        SetActorRotation(FQuat(FRotator(GetVelocity().Rotation())));
        AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
    }
}

void ALMADefaultCharacter::MoveRight(float Value)
{
    if (!IsSprinting)
    {
        AddMovementInput(FVector(0.0f, 1.0f, 0.0f), Value);
    }
    else
    {
        SetActorRotation(FQuat(FRotator(GetVelocity().Rotation())));
        AddMovementInput(FVector(0.0f, 1.0f, 0.0f), Value);
    }
}

void ALMADefaultCharacter::OnSprint()
{
    if (GetVelocity().Length() > 299.0f)
    {
        IsSprinting = true;
        GetCharacterMovement()->MaxWalkSpeed = 750.0f;
        GetWorld()->GetTimerManager().SetTimer(StaminaTimerHandle, this, &ALMADefaultCharacter::DrainStamina, 0.07f, true);
        SetActorRotation(FQuat(FRotator(GetVelocity().Rotation())));
    }
}

void ALMADefaultCharacter::SprintOff()
{
    IsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = 300.0f;
    GetWorld()->GetTimerManager().ClearTimer(StaminaTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(StaminaTimerHandle, this, &ALMADefaultCharacter::RegenStamina, 0.07f, true);
}

void ALMADefaultCharacter::DrainStamina()
{
    Stamina = FMath::Clamp(Stamina - 2.0f, 0.0f, StaminaMax);
    if (Stamina < 0.001 || GetVelocity().Length() < 299.0f)
    {
        SprintOff();
    }
}

void ALMADefaultCharacter::RegenStamina()
{
    if (Stamina == StaminaMax)
    {
        GetWorld()->GetTimerManager().ClearTimer(StaminaTimerHandle);
    }
    Stamina = FMath::Clamp(Stamina + 1.6f, 0.0f, StaminaMax);
}

void ALMADefaultCharacter::CameraZoom(float Value)
{
    if (Value > 0)
    {
        ArmLength -= 100;
    }
    else if (Value < 0)
    {
        ArmLength += 100;
    }
    ArmLength = FMath::Clamp(ArmLength, ArmLengthMin, ArmLengthMax);
    SpringArmComponent->TargetArmLength = ArmLength;
}

void ALMADefaultCharacter::OnDeath()
{
    CurrentCursor->DestroyRenderState_Concurrent();
    PlayAnimMontage(DeathMontage);
    GetCharacterMovement()->DisableMovement();
    SetLifeSpan(5.0f);
    if (IsValid(Controller))
    {
        Controller->ChangeState(NAME_Spectating);
    }
}

void ALMADefaultCharacter::RotationPlayerOnCursor()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (IsValid(PC))
    {
        FHitResult ResultHit;
        PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
        float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
        SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
        if (IsValid(CurrentCursor))
        {
            CurrentCursor->SetWorldLocation(ResultHit.Location);
        }
    }
}

void ALMADefaultCharacter::OnHealthChanged(float NewHealth)
{
    HealthChangedNotify();
}
