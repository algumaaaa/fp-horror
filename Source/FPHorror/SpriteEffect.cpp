
#include "SpriteEffect.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Engine.h"

ASpriteEffect::ASpriteEffect()
{
	PrimaryActorTick.bCanEverTick = false;
	Sprite = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Sprite"));
	Sprite->SetupAttachment(GetRootComponent());
}

void ASpriteEffect::BeginPlay()
{
	Super::BeginPlay();
}

void ASpriteEffect::SetSprite(int Value)
{
	switch (Value) {
		case BulletHole:
			Sprite->SetFlipbook(SpriteBulletHole);
			break;
		case MuzzleFlash:
			Sprite->SetFlipbook(SpriteMuzzleFlash);
			SetActorScale3D(FVector(.5f, .5f, .5f));
			PrimaryActorTick.bCanEverTick = true;
			GetWorld()->GetTimerManager().SetTimer(AnimationTimer, this, &ASpriteEffect::OnTimerTimeOut, Sprite->GetFlipbook()->GetTotalDuration(), false);
			break;
	}
}

void ASpriteEffect::OnTimerTimeOut()
{
	Destroy();
}