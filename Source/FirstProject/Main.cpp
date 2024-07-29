// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include <Kismet/GameplayStatics.h>
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "FirstSaveGame.h"
#include "ItemStorage.h"
#include "MainPlayerController.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom (pulls toward the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;   // Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Set Size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(48.46f, 88.0f);


	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach Cam to the end of the Boom and let the boom adjust to match
	// the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	// Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;


	// Don't rotate when the controller
	// Let that just affect the camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// config charcter movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // .. at this rotation rate

	GetCharacterMovement()->JumpZVelocity = 480.f;
	GetCharacterMovement()->AirControl = 0.2f;


	
	 MaxHealth = 100.f;

	
	 Health = 70.f;

	
	 Stamina = 120.f;

	
	 Coins = 0;

	
     MaxStamina = 150.f;


	 RunningSpeed = 650.f;

	 SprintingSpeed = 950.f;


	 bShiftKeyDown = false;

	 // initializing enums
	 MovementStatus = EMovementStatus::EMS_Normal;
	 StaminaStatus = EStaminaStatus::ESS_Normal;


	 StaminaDrainRate = 25.f;
	 MinSprintStamina = 50.f;
	 InterpSpeed = 15.f;
	 bInterpToEnemy = false;

	 b_E_KeyDown = false;
	 bLMBDown = false;
	 bESCDown = false;

	 bMovingForward = false;
	 bMovingRight = false;

}

void AMain::ShowPickupLocations()
{
	for (int32 i = 0; i < PickupLocations.Num(); i++) {
		UKismetSystemLibrary::DrawDebugSphere(this,PickupLocations[i], 60.f, 8, FLinearColor::Green, 100.f, 2.5f);
	}
}


// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());
	
	// FString Map = GetWorld()->GetMapName();
	// Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

		LoadGameNoSwitch();

		if (MainPlayerController)
		{
			MainPlayerController->GameModeOnly();
		}
	
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus) {

	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown) {
			if (Stamina - DeltaStamina <= MinSprintStamina) {
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStamina;
			}
			else {
				Stamina -= DeltaStamina;
			}
			if (bMovingForward || bMovingRight)
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else {
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
		}
		else {            // Shift Key Up
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}

			else {
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}

		break;

	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown) {
			if (Stamina - DeltaStamina <= 0.f) {
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else {

				Stamina -= DeltaStamina;
				if (bMovingForward || bMovingRight)
				{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else {
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
			}

		}
		else {
			if (Stamina + DeltaStamina >= MinSprintStamina) {
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}

			else {
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		
		break;

	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			Stamina = 0.f;
		}
		else {
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);

		break;

	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinSprintStamina) {
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			Stamina += DeltaStamina;
		}
		else {
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;
     
	default:
		;
	
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMain::E_KeyDown);
	PlayerInputComponent->BindAction("Equip", IE_Released, this, &AMain::E_KeyUp);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &AMain::LMBUp);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMain::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMain::Lookup);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);
}

void AMain::MoveForward(float Value)
{
	bMovingForward = false;

	if (CanMove(Value))
	{   

		// Find out which way is Forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		bMovingForward = true;
	}
}

void AMain::MoveRight(float Value)
{
	bMovingRight = false;
	if (CanMove(Value))
	{
		// Find out which way is Forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
		bMovingRight = true;
	}
}

void AMain::Turn(float Value)
{
	if (CanMove(Value))
	{
		AddControllerYawInput(Value);
	}
}

void AMain::Lookup(float Value)
{

	if (CanMove(Value))
	{
		AddControllerPitchInput(Value);
	}
}

bool AMain::CanMove(float Value)
{
	if (MainPlayerController)
	{
		return (Controller != nullptr)
			&& (Value != 0.0f) &&
			!(bAttacking) &&
			(MovementStatus != EMovementStatus::EMS_Dead) &&
			!MainPlayerController->bPauseMenuVisible;
	}

	return false;

}

void AMain::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::E_KeyDown()
{
	b_E_KeyDown = true;

	if (ActiveOverlappingItem) {
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}
}

void AMain::E_KeyUp()
{
	b_E_KeyDown = false;
}

void AMain::LMBDown()
{
	bLMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;

	if (EquippedWeapon)
	{
		Attack();

	}

}

void AMain::LMBUp()
{
	bLMBDown = false;
}



void AMain::ESCUp()
{
	bESCDown = false;
}


void AMain::ESCDown()
{
	bESCDown = true;

	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}

void AMain::DecrementHealth(float Amount)
{
	if (Health - Amount <= 0.f)
	{
		Health -= Amount;
		Die();
    }

	else {
		Health -= Amount;
	}
	
}

float AMain::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (Health - DamageAmount <= 0)
	{
		Health -= DamageAmount;
		Die();
		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	}
	else {
		Health -= DamageAmount;
	}
	return DamageAmount;
}

void AMain::Die()
{

	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));

	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

void AMain::Jump()
{
	if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;

	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		Super::Jump();
	}
}

void AMain::IncrementCoins(int32 Amount)
{
	if (Coins < 999)
	{
		Coins += Amount;
	}
}

void AMain::IncrementHealth(float Amount)
{
	if (Health + Amount >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else {
		Health += Amount;
	}
}


void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}

}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon) 
	{ 
		EquippedWeapon->Destroy();
	
	
	}
	EquippedWeapon = WeaponToSet;
}

void AMain::Attack()
{

	if (!bAttacking && (MovementStatus != EMovementStatus::EMS_Dead)) {
		
		bAttacking = true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{

			int32 Section = FMath::RandRange(0, 1);
			switch (Section)
			{
			case 0:

				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);

				break;

			case 1:

				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);

				break;

			default:
				;

			}
			
		}

	}


}

void AMain::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);
	if (bLMBDown)
	{
		Attack();
	}
}

void AMain::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound) {
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

void AMain::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMain::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0)
	{
		/*if (MainPlayerController)
		{
			MainPlayerController->RemoveEnemyHealthBar();
		}*/
		return;
	}

	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);

	if (ClosestEnemy)
	{
		FVector Location = GetActorLocation();
		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();

		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}
		//if (MainPlayerController)
		//{
			// MainPlayerController->DisplayEnemyhealthBar();
		//}
		SetCombatTarget(ClosestEnemy);
		// bHasCombatTarget = true;

	}

	
}

void AMain::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();

		FName CurrentLevelName(*CurrentLevel);

		if (CurrentLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void AMain::SaveGame()
{
	UFirstSaveGame* SaveGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
	SaveGameInstance->CharacterStats.Coins = Coins;

	FString MapName = GetWorld()->GetMapName();
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix); // removes prefix for map name

	SaveGameInstance->CharacterStats.LevelName = MapName;

	if (EquippedWeapon)
	{
		SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
	}



	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);

}

void AMain::LoadGame(bool SetPosition)
{
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));

	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
	Coins = LoadGameInstance->CharacterStats.Coins;

	if (WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

			if (Weapons->WeaponMap.Contains(WeaponName))
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
				WeaponToEquip->Equip(this);
			}
		}
	}

	if (SetPosition)
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}

	SetMovementStatus(EMovementStatus::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;

	if (LoadGameInstance->CharacterStats.LevelName != TEXT(""))
	{
		FName LevelName(*LoadGameInstance->CharacterStats.LevelName);

		SwitchLevel(LevelName);
	}

}

void AMain::LoadGameNoSwitch()
{
	// Create a new SaveGameInstance and attempt to load the game
	UFirstSaveGame* LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::CreateSaveGameObject(UFirstSaveGame::StaticClass()));
	LoadGameInstance = Cast<UFirstSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	// Update character stats based on the loaded game data
	if (LoadGameInstance)
	{
		Health = LoadGameInstance->CharacterStats.Health;
		MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
		Stamina = LoadGameInstance->CharacterStats.Stamina;
		MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
		Coins = LoadGameInstance->CharacterStats.Coins;

		// Check if WeaponStorage exists and spawn and equip weapons
		if (WeaponStorage)
		{
			AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
			if (Weapons)
			{
				FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

				if (Weapons->WeaponMap.Contains(WeaponName))
				{
					AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
					if (WeaponToEquip)
					{
						WeaponToEquip->Equip(this);
					}
				}
			}
		}

		// Reset movement status and animation updates
		SetMovementStatus(EMovementStatus::EMS_Normal);
		GetMesh()->bPauseAnims = false;
		GetMesh()->bNoSkeletonUpdate = false;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load the game. Save game instance is null."));
	}
}

