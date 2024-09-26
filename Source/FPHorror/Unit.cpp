
#include "Unit.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Global.h"

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	UnitFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Unit Flipbook"));
	UnitFlipbook->SetupAttachment(GetRootComponent());
	
	Health = 100.f;
	LastSpriteOutput = -1;
	bIsSpriteFlipped = false;
}

void AUnit::BeginPlay()
{
	Super::BeginPlay();
	
}

void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UnitFacePlayer();
	SpriteFacePlayer();
	SpriteOrientFromPlayer();
}

void AUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AUnit::ChangeHealth(int Value)
{
	Health = std::max(Health += Value, 0);
}

int AUnit::GetHealth()
{
	return Health;
}

FRotator AUnit::GetRotationTowardsPlayer()
{
	FVector CamLoc = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
	FVector UnitLoc = GetActorLocation();
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(UnitLoc, CamLoc);
	LookAtRot.Yaw -= 90.f;
	FRotator CurrRot = GetActorRotation();
	FRotator NewRot = FRotator(CurrRot.Pitch, LookAtRot.Yaw, CurrRot.Roll);
	return NewRot;
}

void AUnit::UnitFacePlayer()
{
	SetActorRotation(GetRotationTowardsPlayer());
}

void AUnit::SpriteFacePlayer()
{
	FRotator RotToPlayer = GetRotationTowardsPlayer();
	if (bIsSpriteFlipped) RotToPlayer.Yaw -= 180;
	UnitFlipbook->SetRelativeRotation(RotToPlayer);
}

void AUnit::SpriteOrientFromPlayer()
{
	FRotator CamRot = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation();
	FVector CamForwardVec = UKismetMathLibrary::GetForwardVector(CamRot);
	FVector UnitForwardVec = GetActorForwardVector();
	FVector UnitRightVec = GetActorRightVector();
	float RightDot = FVector::DotProduct(UnitRightVec, CamForwardVec);
	float ForwardDot = FVector::DotProduct(UnitForwardVec, CamForwardVec);
	int SpriteOutput = 0;

	if (ForwardDot < -0.85) {
		SpriteOutput = 0;
	}
	else if (ForwardDot > 0.85) {
		SpriteOutput = 4;
	}
	else {
		bIsSpriteFlipped = RightDot < 0;

		if (abs(ForwardDot) < 0.3) {
			SpriteOutput = 2;
		}
		else if (ForwardDot < 0) {
			SpriteOutput = 1;
		}
		else {
			SpriteOutput = 3;
		}
	}
	if (SpriteOutput != LastSpriteOutput) {
		LastSpriteOutput = SpriteOutput;
		UnitFlipbook->SetFlipbook(SpritesIdle[SpriteOutput]);
	}
}