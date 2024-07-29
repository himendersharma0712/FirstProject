// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstance2.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UAnimInstance2 : public UAnimInstance
{
	GENERATED_BODY()



public:

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = AnimaionProperties)
	void UpdateAnimationProperties();


	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Movement)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Movement)
	bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Movement)
	class APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Movement)
	class AMain* Main;

	
};
