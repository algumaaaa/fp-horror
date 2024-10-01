
#include "Unit.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Global.h"
#include "PaperZDAnimInstance.h"

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	UnitFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Unit Flipbook"));
	UnitFlipbook->SetupAttachment(GetRootComponent());
	
	Health = 100.f;
	LastSpriteOutput = -1;
	bIsSpriteFlipped = false;
	CurrState = Idle;
	SpriteVector = FVector2D(0.f, 1.f);
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
	ExecCurrState();
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

void AUnit::HurtUnit(int Value)
{
	// Ensure its negative
	Value = -(abs(Value));
	if (CanEnterState(Hurt)) {
		ChangeToState(Hurt);
		ChangeHealth(Value);
	}
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
		//UnitFlipbook->SetFlipbook(SpritesIdle[SpriteOutput]);
	}
}

void AUnit::UpdateSpriteAngleVar()
{
	switch (LastSpriteOutput) {
		case 0:
			SpriteVector = FVector2D(0.f, 1.f);
			return;
		case 1:
			SpriteVector = FVector2D(0.9f, 0.4f);
			return;
		case 2:
			SpriteVector = FVector2D(0.5f, -0.8f);
			return;
		case 3:
			SpriteVector = FVector2D(-0.5f, -0.8f);
			return;
		case 4:
			SpriteVector = FVector2D(-0.9f, 0.4f);
			return;
	}
}


///////////////////
/* STATE MACHINE */
///////////////////

int AUnit::GetCurrentState()
{
	return CurrState;
}

void AUnit::ChangeToState(int Value)
{
	// Exec state entering procedures
	switch (Value) {
		case Death:
			// Works for now. Might be better to manually set a collision channel?
			SetActorEnableCollision(false);
			AnimInstance->JumpToNode("WasKilled");
			break;
		case Idle:
			AnimInstance->JumpToNode("Out");
			break;
		case Hurt:
			AnimInstance->JumpToNode("WasHurt");
			break;
	}
	CurrState = Value;
}

bool AUnit::CanEnterState(int Value)
{
	switch(CurrState) {
		case Death:
			return true;
		case Idle:
			if (CurrState > Death) return true;
		case Chase:
			if (CurrState > Death && CurrState < Attack) return true;
		case Attack:
			if (CurrState > Idle && CurrState < Hurt) return true;
		case Hurt:
			if (CurrState > Death) return true;
	}
	return false;
}

void AUnit::ExecCurrState()
{
	switch(CurrState) {
		case Death:
			StateDeath();
			break;
		case Idle:
			StateIdle();
			break;
		case Chase:
			StateChase();
			break;
		case Attack:
			StateAttack();
			break;
		case Hurt:
			StateHurt();
			break;
	}
}

void AUnit::StateDeath()
{
	// Timer 'til it calls Destroy();
	return;
}

void AUnit::StateIdle()
{
	// Check if unit needs to leave idle
	return;
}

void AUnit::StateChase()
{

}

void AUnit::StateAttack()
{
	// Wait for anim to finish before leaving state
	// During anim dmg frames, check if can dmg player. if so disable damaging 'til state is left
	// Might need a LeaveState() func for this?
}

void AUnit::StateHurt()
{
	// Waiting for hurt animation to finish before transitioning back to idle
	if (Health == 0 && CanEnterState(Death)) ChangeToState(Death);
}