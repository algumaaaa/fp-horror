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
	UPROPERTY(BlueprintReadWrite, Category = "Sprite")
	class UPaperFlipbook* SpriteBulletHole;
	UFUNCTION(BlueprintCallable, Category = "Sprite")
	void SetSprite(int Value);

	enum SpriteName {
		BulletHole = 1,
	};

protected:
	virtual void BeginPlay() override;
	class UPaperFlipbookComponent* Sprite;
};
