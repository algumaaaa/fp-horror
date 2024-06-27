// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpriteEffect.generated.h"

UCLASS()
class FPHORROR_API ASpriteEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpriteEffect();

	// Must be a cleaner way to do these references, but TArray doesnt work cause this is a great engine
	UPROPERTY(EditAnywhere, Category = "Sprite")
	class UPaperFlipbook* SpriteBulletHole;
	UPROPERTY(EditAnywhere, Category = "Sprite")
	class UPaperFlipbook* SpriteMuzzleFlash;
	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void SetSprite(int Value);

	void OnTimerTimeOut();
	FTimerHandle AnimationTimer;

	enum SpriteName {
		BulletHole = 1,
		MuzzleFlash = 2,
	};

protected:
	virtual void BeginPlay() override;
	class UPaperFlipbookComponent* Sprite;
};
