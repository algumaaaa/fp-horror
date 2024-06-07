
#include "SpriteEffect.h"
#include "PaperFlipbookComponent.h"

ASpriteEffect::ASpriteEffect()
{
	PrimaryActorTick.bCanEverTick = true;
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
	}
}