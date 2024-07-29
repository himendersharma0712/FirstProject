// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	/** REference to the UMG asset in the Editor  */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;

	// Variable to hold the widget after creating it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> WPauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	UUserWidget* PauseMenu;

	bool bPauseMenuVisible;

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable,Category = "HUD")
	void DisplayPauseMenu();

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "HUD")
	void RemovePauseMenu(); 


	void TogglePauseMenu();

	void GameModeOnly();

protected:
	virtual void BeginPlay() override;
	
};
