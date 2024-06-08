
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Unit.generated.h"

UCLASS()
class FPHORROR_API AUnit : public ACharacter
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	AUnit();
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void ChangeHealth(int Value);

protected:
	virtual void BeginPlay() override;

	int Health;

};
