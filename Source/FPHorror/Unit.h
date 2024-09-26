
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
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Sprite")
	class UPaperFlipbookComponent* UnitFlipbook;
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void ChangeHealth(int Value);
	int GetHealth();

	UPROPERTY(EditAnywhere, Category = "Sprite")
	TArray<UPaperFlipbook*> SpritesIdle;

protected:
	virtual void BeginPlay() override;

	FRotator GetRotationTowardsPlayer();
	void UnitFacePlayer();
	void SpriteFacePlayer();
	void SpriteOrientFromPlayer();

	int Health;
	int LastSpriteOutput;
	bool bIsSpriteFlipped;

};
