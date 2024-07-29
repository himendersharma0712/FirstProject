// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"


UENUM(BlueprintType)
enum class EMovementStatus : uint8                                             // scoped enum can be declared with enum prefix
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead     UMETA(DisplayName = "Dead"),
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};


UENUM(BlueprintType)
enum class EStaminaStatus: uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMax")

};

UCLASS()
class FIRSTPROJECT_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(EditDefaultsOnly, Category = "SavedData")
	TSubclassOf<class AItemStorage> WeaponStorage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Item")
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	class AItem* ActiveOverlappingItem;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus  StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MinSprintStamina;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Enums")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enums")
	float SprintingSpeed;

	bool bShiftKeyDown;

    float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);


	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target;  }

	FRotator GetLookAtRotationYaw(FVector Target);
	// Pressed down to enable Spriniting
	void ShiftKeyDown();

	// Released to disable Spriniting
	void ShiftKeyUp();



	// Set Movement status  for this character's properties
	void SetMovementStatus(EMovementStatus Status);

	/**  Camera boom positioning the camera behind the Player */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Camera,meta =(AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// Base turn rates to scale turning functions for the camera 
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Camera)
	float BaseTurnRate;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	
	// Player Stats
	

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	int32 Coins;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStamina;

	
	void DecrementHealth(float Amount);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Die();

	virtual void Jump() override;

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	TArray<FVector> PickupLocations;    // array template

	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();        // Function for pinning locations

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value); // for forward and backward movement
	void MoveRight(float Value); // for moving left and right

	void Turn(float Value); // for yaw rotation
	void Lookup(float Value); // for pitch rotation


	bool bMovingForward;
	bool bMovingRight;

	bool CanMove(float Value);
	
	
	// @param Rate this is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	void TurnAtRate(float Rate); 
	// @param Rate this is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate
	void LookUpAtRate(float Rate);

	bool b_E_KeyDown;

	void E_KeyDown();

	void E_KeyUp();

	// Left Mouse button

	void LMBDown();

	void LMBUp();

	bool bLMBDown;


	// Escape Key for Pause Menu

	void ESCDown();

	void ESCUp();

	bool bESCDown;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; };
	void SetEquippedWeapon(AWeapon* WeaponToSet);
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; };

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	void UpdateCombatTarget();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	UFUNCTION(BlueprintCallable)
	void LoadGameNoSwitch();


};
