// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPHorrorGameMode.h"
#include "FPHorrorCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPHorrorGameMode::AFPHorrorGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
