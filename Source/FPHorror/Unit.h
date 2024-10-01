
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Unit.generated.h"

class UPaperFlipbook;

UCLASS()
class FPHORROR_API AUnit : public ACharacter
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	AUnit();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprite")
	class UPaperZDAnimInstance* AnimInstance;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Sprite")
	class UPaperFlipbookComponent* UnitFlipbook;
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void ChangeHealth(int Value);
	int GetHealth();
	void HurtUnit(int Value);

	UPROPERTY(EditAnywhere, Category = "Sprite")
	TArray<UPaperFlipbook*> SpritesIdle;

	UPROPERTY(BlueprintReadOnly, Category = "Sprite")
	FVector2D SpriteVector;

	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void UpdateSpriteAngleVar();
	
	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	int GetCurrentState();

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void ChangeToState(int Value);

protected:
	virtual void BeginPlay() override;

	FRotator GetRotationTowardsPlayer();
	void UnitFacePlayer();
	void SpriteFacePlayer();
	void SpriteOrientFromPlayer();

	int Health;
	bool bIsSpriteFlipped;
	int LastSpriteOutput;
	
	/* STATE MACHINE */
	bool CanEnterState(int Value);
	void ExecCurrState();
	void StateDeath();
	void StateIdle();
	void StateChase();
	void StateAttack();
	void StateHurt();
	int CurrState;
	/* STATE MACHINE */
	enum State {
		Death = 1,
		Idle = 2,
		Chase = 4,
		Attack = 8,
		Hurt = 16,
	};
};
