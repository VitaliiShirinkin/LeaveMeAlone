// LeaveMeAlone Game by Netologiya. All RightsReserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LMADefaultCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ULMAHealthComponent;
class UAnimMontage;
class ULMAWeaponComponent;

UCLASS()
class LEAVEMEALONE_API ALMADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALMADefaultCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    UCameraComponent* CameraComponent;

	UPROPERTY()
    UDecalComponent* CurrentCursor = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
    UMaterialInterface* CursorMaterial = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
    FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components|Health")
    ULMAHealthComponent* HealthComponent;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* DeathMontage;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    ULMAWeaponComponent* WeaponComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION()
    ULMAHealthComponent* GetHealthComponent() const { return HealthComponent; }

    UFUNCTION(BlueprintImplementableEvent)
    void HealthChangedNotify();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float Stamina = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float StaminaMax = 100.0f;

private:
    float YRotation = -75.0f;
    float ArmLength = 1200.0f;
    const float ArmLengthMin = 400.0f;
    const float ArmLengthMax = 1600.0f;
    float FOV = 55.0f;
    FTimerHandle StaminaTimerHandle;
    bool IsSprinting = false;

	void MoveForward(float Value);
    void MoveRight(float Value);
    void OnSprint();
    void SprintOff();
    void DrainStamina();
    void RegenStamina();
    void CameraZoom(float Value);
    void OnDeath();
    void RotationPlayerOnCursor();
    void OnHealthChanged(float NewHealth);
};
