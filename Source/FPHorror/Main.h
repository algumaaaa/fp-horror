
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

class ASpriteEffect;
class UCameraShakeBase;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite")
	FVector GunOffset;
	UPROPERTY(EditAnywhere, Category = "Sprite")
	TSubclassOf<ASpriteEffect> SpriteEffect;
	
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* MuzzleLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FirstPersonCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float TurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float LookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	class USpringArmComponent* CameraBoom;
	UPROPERTY(EditAnywhere, Category = "Camera")
	TSubclassOf<UCameraShakeBase> CameraShake;
	
	/* GAMEPLAY */

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int LoadedBullets;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int Ammo;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	float CrosshairOffset;

	/* STATE MACHINE */
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void ChangeState(int Value);
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	int GetCurrentState();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool bIsStateIdleWalk;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool bIsStateAim;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	int QueuedInput;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay")
	bool bIsAimPressed;

protected:
	void OnFire();
	void OnReload();
	void OnAim();
	void OnRun();
	void StopRunning();
	void StopAiming();
	void ChangeCrosshairOffset(float Value);
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookAtRate(float Rate);

	/* ANIMATION */
	void AnimateLookOffset(float Rate);
	void AnimateGunSway(float Rate);
	void AnimateAimDownSight(bool bToggle);
	FVector SwayOffset;
	float ArmOffset;
	float SwayThreshhold;
	float AnimPosGunSway;
	float AnimPosADS;
	
	/* STATE MACHINE */
	bool CanEnterState(int Value);
	int CurrState;
	enum State {
		IdleWalk = 1,
		Run = 2,
		Aim = 4,
		Fire = 8,
		Reload = 16,
	};
};
