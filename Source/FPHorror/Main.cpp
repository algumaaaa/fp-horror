
#include "Main.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "PaperFlipbookComponent.h"
#include "Global.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PaperZDAnimInstance.h"


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
	MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	GunOffset = FVector(100.f, 0.f, 10.f);

	ArmOffset = CameraBoom->GetRelativeLocation().X;
	SwayThreshhold = .5f;
	ADSAnimProg = 0;

	bIsRunning = false;
	bIsReadyingGun = false;
	bCanShoot = false;
	bIsReloading = false;

	CurrState = IdleWalk;

	LoadedBullets = 6;
	Ammo = 30;
}

void AMain::BeginPlay()
{
	Super::BeginPlay();
}

void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Is there a way to do this using signals? Probably doesnt matter much performance wise
	if (bIsReadyingGun || ADSAnimProg > 0) AnimateAimDownSight(bIsReadyingGun);
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
	if (LoadedBullets > 0 && bCanShoot && AnimInstance != nullptr) {
		AnimInstance->JumpToNode("Shoot");
		LoadedBullets -= 1;
	}
}

void AMain::OnReload()
{
	if (!bIsReloading && bCanShoot && AnimInstance != nullptr){
		AnimInstance->JumpToNode("Reload");
		bIsReloading = true;
	}
}

void AMain::OnAim()
{
	bIsReadyingGun = true;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
}

void AMain::StopAiming()
{
	bIsReadyingGun = false;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

void AMain::OnRun()
{
	if (bCanShoot || bIsReadyingGun) return;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	bIsRunning = true;
}

void AMain::StopRunning()
{
	if (bCanShoot || bIsReadyingGun) return;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	bIsRunning = false;
}

void AMain::MoveForward(float Value)
{
	if (Value == 0.f) return;
	AddMovementInput(GetActorForwardVector(), Value);
	AnimateGunSway(GetCharacterMovement()->MaxWalkSpeed / 300.f);
}

void AMain::MoveRight(float Value)
{
	if (Value == 0.f) return;
	AddMovementInput(GetActorRightVector(), Value);
	if (GetInputAxisValue("MoveForward") == 0.f) {
		AnimateGunSway(GetCharacterMovement()->MaxWalkSpeed / 300.f);
	}
}

void AMain::TurnAtRate(float Rate)
{
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	AddControllerYawInput(Rate * TurnRate * DeltaTime);
	// Move this down to a look offset func
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

void AMain::LookAtRate(float Rate)
{
	AddControllerPitchInput(Rate * LookUpRate * GetWorld()->GetDeltaSeconds());
}

// TODO: Move this down to a gun script?
void AMain::AnimateGunSway(float Rate)
{
	FVector CurrLoc = GunFlipbook->GetRelativeLocation();
	if (AnimPos < 1) { 
		GunFlipbook->SetRelativeLocation(FVector(CurrLoc.X - Rate*.1f, CurrLoc.Y, CurrLoc.Z + Rate*.07f)); 
		}
	else if (AnimPos < 2) { 
		GunFlipbook->SetRelativeLocation(FVector(CurrLoc.X - Rate*.1f, CurrLoc.Y, CurrLoc.Z - Rate*.07f));
		}
	else if (AnimPos < 3) { 
		GunFlipbook->SetRelativeLocation(FVector(CurrLoc.X + Rate*.1f, CurrLoc.Y, CurrLoc.Z + Rate*.07f));
		}
	else if (AnimPos < 4) { 
		GunFlipbook->SetRelativeLocation(FVector(CurrLoc.X + Rate*.1f, CurrLoc.Y, CurrLoc.Z - Rate*.07f));
		}
	else AnimPos = 0;
	AnimPos += 2 * Rate * GetWorld()->GetDeltaSeconds();
}

void AMain::AnimateAimDownSight(bool bToggle)
{
	if (bToggle && ADSAnimProg >= 1) return;
	FVector CurrLoc = GunFlipbook->GetRelativeLocation();
	float OffsetY = .02f * float(Global::EaseOutExpo(double(ADSAnimProg)));
	OffsetY = bToggle ? OffsetY : OffsetY * -1;
	if (bToggle) {
		ADSAnimProg += 1 * GetWorld()->GetDeltaSeconds();
	}
	else if (ADSAnimProg > 0) {
		ADSAnimProg -= 1 * GetWorld()->GetDeltaSeconds();
	}
	ArmOffset += OffsetY;
	GunFlipbook->SetRelativeLocation(FVector(CurrLoc.X, CurrLoc.Y - OffsetY, CurrLoc.Z));
}

