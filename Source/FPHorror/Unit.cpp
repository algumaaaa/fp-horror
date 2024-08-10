
#include "Unit.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/KismetMathLibrary.h"

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	UnitFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Unit Flipbook"));
	UnitFlipbook->SetupAttachment(GetRootComponent());
	
	Health = 100.f;
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
	UnitFlipbook->SetRelativeRotation(GetRotationTowardsPlayer());
}