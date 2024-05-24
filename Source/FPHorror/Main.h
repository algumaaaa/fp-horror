
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UCLASS()
class FPHORROR_API AMain : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	AMain();
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite")
	class UPaperZDAnimInstance* AnimInstance;

	UPROPERTY(VisibleDefaultsOnly, Category = "Sprite")
	class UPaperFlipbookComponent* GunFlipbook;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* MuzzleLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float TurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float LookUpRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	FVector GunOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	bool bIsReadyingGun;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool bCanShoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool bIsReloading;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int LoadedBullets;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int Ammo;
	// Needed??
	bool bIsRunning;

protected:
	void OnFire();
	void OnReload();
	void OnAim();
	void OnRun();
	void StopRunning();
	void StopAiming();
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookAtRate(float Rate);
	void AnimateGunSway(float Rate);
	void AnimateAimDownSight(bool bToggle);

	float ArmOffset;
	float SwayThreshhold;
	// TODO: Change the name of this to smt clearer
	float AnimPos;
	float ADSAnimProg;
	
	int CurrState;
	enum States {
		IdleWalk = 1,
		Run = 2,
		Aim = 4,
		Fire = 8,
		Reload = 16,
	};
};
