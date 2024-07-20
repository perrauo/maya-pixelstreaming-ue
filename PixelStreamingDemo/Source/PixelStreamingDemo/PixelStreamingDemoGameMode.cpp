// Copyright Epic Games, Inc. All Rights Reserved.

#include "PixelStreamingDemoGameMode.h"
#include "PixelStreamingDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"


APixelStreamingDemoGameMode::APixelStreamingDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
