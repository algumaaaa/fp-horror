
#include "Main.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "PaperFlipbookComponent.h"
#include "Global.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PaperZDAnimInstance.h"
#include "DrawDebugHelpers.h"
#include "SpriteEffect.h"
#include "Unit.h"
#include "Kismet/KismetMathLibrary.h"


AMain::AMain()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(40.f, 95.f);

	TurnRate = 45.f;
	LookUpRate = 45.f;
	
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->AddRelativeLocation(FVector(-39.65f, 1.75f, 64.f));
	FirstPersonCamera->bUsePawnControlRotation = true;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->TargetArmLength = 0.f;
	CameraBoom->SetupAttachment(FirstPersonCamera);
	
	GunFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Gun Flipbook"));
	GunFlipbook->SetupAttachment(CameraBoom);

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle Location"));
	MuzzleLocation->SetupAttachment(FirstPersonCamera);
	MuzzleLocation->SetRelativeLocation(FVector(52.1f, 16.3f, .6f));

	GunOffset = FVector(100.f, 0.f, 10.f);

	SwayThreshhold = .5f;
	AnimPosADS = 0;

	QueuedInput = -1;
	bIsAimPressed = false;
	CurrState = IdleWalk;
	LoadedBullets = 6;
	Ammo = 30;
	CrosshairOffset = 0.f;
}

void AMain::BeginPlay()
{
	Super::BeginPlay();
	ArmOffset = CameraBoom->GetRelativeLocation().X;
	SwayOffset = GunFlipbook->GetRelativeLocation();
}

void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Is there a way to do this using signals? Probably doesnt matter much performance wise
	if (GetCurrentState() == Aim || AnimPosADS > 0) AnimateAimDownSight(bIsStateAim);
	CrosshairOffset = CrosshairOffset > 0.f ? CrosshairOffset -= 30.f * GetWorld()->GetDeltaSeconds() : 0.f;
}

void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMain::OnFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMain::OnReload);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AMain::OnAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AMain::StopAiming);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AMain::OnRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AMain::StopRunning);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AMain::LookAtRate);
	PlayerInputComponent->BindAxis("LookRight", this, &AMain::TurnAtRate);
}

void AMain::OnFire()
{
	if (CanEnterState(Fire)) {
		ChangeToState(Fire);

		// Linetracing
		FHitResult HitResult;
		FVector Start = FirstPersonCamera->GetComponentLocation();
		FVector Forward = FirstPersonCamera->GetForwardVector();
		Forward = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(Forward, 10 * (CrosshairOffset/100));

		FVector End = Start + Forward * 20000.f;

		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
		AActor* ResultActor = HitResult.GetActor();
		AUnit* ResultUnit = Cast<AUnit>(ResultActor);
	
		// Hit unit
		if (ResultUnit != nullptr) {
			ResultUnit->HurtUnit(51);
			Global::Print(FString::FromInt(ResultUnit->GetHealth()));
		}
		
		// Bullet hole
		else if (HitResult.bBlockingHit && SpriteEffect != nullptr) {
			const FVector Loc = HitResult.Location + HitResult.Normal;
			const FRotator Rot = FRotationMatrix::MakeFromY(HitResult.Normal).Rotator();
			ASpriteEffect* BulletHole = GetWorld()->SpawnActor<ASpriteEffect>(SpriteEffect, Loc, Rot);
			BulletHole->SetSprite(ASpriteEffect::BulletHole);
		}

		// Muzzle flash
		FVector Loc = MuzzleLocation->GetComponentLocation();
		FRotator Rot = MuzzleLocation->GetComponentRotation();
		Rot.Yaw += 90.f;
		ASpriteEffect* MuzzleFlash = GetWorld()->SpawnActor<ASpriteEffect>(SpriteEffect, Loc, Rot);
		MuzzleFlash->SetSprite(ASpriteEffect::MuzzleFlash);
		MuzzleFlash->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform, "NAME_none");
		
		// TODO: Change value to var exposed to editor
		ChangeCrosshairOffset(100);
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayWorldCameraShake(GetWorld(), CameraShake, GetActorLocation(), 0.f, 500.f, 1.f);
		
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2);
	}
}

void AMain::OnReload()
{
	if (CanEnterState(Reload)) ChangeToState(Reload);
}

void AMain::OnAim()
{	
	bIsAimPressed = true;
	if (CanEnterState(Aim)) {
		ChangeToState(Aim);
		// TODO: Change value to var exposed to editor
		ChangeCrosshairOffset(50);
	} 
	else QueuedInput = 4;
}

void AMain::StopAiming()
{	
	bIsAimPressed = false;
	if (CanEnterState(IdleWalk)) {
		ChangeToState(IdleWalk);
	}
	else QueuedInput = 1;
}

void AMain::OnRun()
{
	if (CanEnterState(Run)) ChangeToState(Run);
}

void AMain::StopRunning()
{
	if (GetCurrentState() == Aim) return;
	if (CanEnterState(IdleWalk)) ChangeToState(IdleWalk);
}

void AMain::MoveForward(float Value)
{
	if (Value == 0.f) return;
	AddMovementInput(GetActorForwardVector(), Value);
	AnimateGunSway(GetCharacterMovement()->MaxWalkSpeed / 300.f);
	// TODO: Change value to var exposed to editor
	ChangeCrosshairOffset(25*GetWorld()->GetDeltaSeconds());
}

void AMain::MoveRight(float Value)
{
	if (Value == 0.f) return;
	AddMovementInput(GetActorRightVector(), Value);
	if (GetInputAxisValue("MoveForward") == 0.f) {
		AnimateGunSway(GetCharacterMovement()->MaxWalkSpeed / 300.f);
		// TODO: Change value to var exposed to editor
		ChangeCrosshairOffset(25*GetWorld()->GetDeltaSeconds());
	}
}

void AMain::TurnAtRate(float Rate)
{
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	AddControllerYawInput(Rate * TurnRate * DeltaTime);
	AnimateLookOffset(Rate);
}

void AMain::LookAtRate(float Rate)
{
	AddControllerPitchInput(Rate * LookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::ChangeCrosshairOffset(float Value)
{
	const float MAX_CROSSHAIR_OFFSET = 100.f;

	CrosshairOffset = CrosshairOffset + Value > MAX_CROSSHAIR_OFFSET ? MAX_CROSSHAIR_OFFSET : CrosshairOffset + Value;
}

///////////////
/* ANIMATION */
///////////////

void AMain::AnimateLookOffset(float Rate)
{
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	FVector CamLoc = CameraBoom->GetRelativeLocation();
	float ReturnStrenght = abs(CamLoc.X - ArmOffset);
	if (Rate > SwayThreshhold) {
		CameraBoom->SetRelativeLocation(FVector(CamLoc.X - Rate*2 * DeltaTime, CamLoc.Y, CamLoc.Z));
	}
	else if (Rate < -SwayThreshhold) {
		CameraBoom->SetRelativeLocation(FVector(CamLoc.X - Rate*2 * DeltaTime, CamLoc.Y, CamLoc.Z));
	}
	else if (CamLoc.X > ArmOffset) {
		CameraBoom->SetRelativeLocation(FVector(CamLoc.X - ReturnStrenght * DeltaTime, CamLoc.Y, CamLoc.Z));
	}
	else if (CamLoc.X < ArmOffset) {
		CameraBoom->SetRelativeLocation(FVector(CamLoc.X + ReturnStrenght * DeltaTime, CamLoc.Y, CamLoc.Z));
	}
}

void AMain::AnimateGunSway(float Rate)
{
	FVector CurrLoc = GunFlipbook->GetRelativeLocation();
	float amtX = Rate * 3.f * GetWorld()->GetDeltaSeconds();
	float amtZ = Rate * 2.7f * GetWorld()->GetDeltaSeconds();
	AnimPosGunSway += Rate * GetWorld()->GetDeltaSeconds();
	
	if (AnimPosGunSway < 1) { 
		GunFlipbook->SetRelativeLocation(FVector(CurrLoc.X - amtX, CurrLoc.Y, CurrLoc.Z + amtZ)); 
		}
	else if (AnimPosGunSway < 2) { 
		GunFlipbook->SetRelativeLocation(FVector(CurrLoc.X - amtX, CurrLoc.Y, CurrLoc.Z - amtZ));
		}
	else if (AnimPosGunSway < 3) { 
		GunFlipbook->SetRelativeLocation(FVector(CurrLoc.X + amtX, CurrLoc.Y, CurrLoc.Z + amtZ));
		}
	else if (AnimPosGunSway < 4) { 
		GunFlipbook->SetRelativeLocation(FVector(CurrLoc.X + amtX, CurrLoc.Y, CurrLoc.Z - amtZ));
		}
	else {
		AnimPosGunSway = 0;
		FVector NewLoc = SwayOffset;
		NewLoc.Y -= ArmOffset;
		GunFlipbook->SetRelativeLocation(NewLoc);
	}
}

void AMain::AnimateAimDownSight(bool bToggle)
{
	if (bToggle && AnimPosADS >= 1) return;
	FVector CurrLoc = GunFlipbook->GetRelativeLocation();
	float OffsetY = .02f * float(Global::EaseOutExpo(double(AnimPosADS)));
	OffsetY = bToggle ? OffsetY : OffsetY * -1;
	if (bToggle) {
		AnimPosADS += 1 * GetWorld()->GetDeltaSeconds();
	}
	else if (AnimPosADS > 0) {
		AnimPosADS -= 1 * GetWorld()->GetDeltaSeconds();
	}
	ArmOffset += OffsetY;
	GunFlipbook->SetRelativeLocation(FVector(CurrLoc.X, CurrLoc.Y - OffsetY, CurrLoc.Z));
}

///////////////////
/* STATE MACHINE */
///////////////////

void AMain::ChangeToState(int Value)
{
	bIsStateIdleWalk = false;
	bIsStateAim = false;
	switch(Value) {
		case IdleWalk:
			bIsStateIdleWalk = true;
			AnimInstance->JumpToNode("Out");
			GetCharacterMovement()->MaxWalkSpeed = 300.f;
			break;
		case Run:
			GetCharacterMovement()->MaxWalkSpeed = 600.f;
			break;
		case Aim:
			bIsStateAim = true;
			GetCharacterMovement()->MaxWalkSpeed = 150.f;
			break;
		case Fire:
			AnimInstance->JumpToNode("Shoot");
			LoadedBullets -= 1;
			break;
		case Reload:
			AnimInstance->JumpToNode("Reload");
			break;
	}
	CurrState = Value;
}

bool AMain::CanEnterState(int Value)
{
	switch (Value) {
		case IdleWalk:
			if (CurrState <= Aim) return true;
		case Run:
			if (CurrState == IdleWalk) return true;
		case Aim:
			if (CurrState <= Aim) return true;
		case Fire:
			if (CurrState == Aim && LoadedBullets > 0 && AnimInstance != nullptr) return true;
		case Reload:
			if (CurrState == Aim && Ammo > 0 && LoadedBullets < 6 && AnimInstance != nullptr) return true;
	}
	return false;
}

int AMain::GetCurrentState()
{
	return CurrState;
}