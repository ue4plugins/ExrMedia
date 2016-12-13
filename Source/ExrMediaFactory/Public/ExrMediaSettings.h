// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "ExrMediaSettings.generated.h"


UCLASS(config=Engine)
class EXRMEDIAFACTORY_API UExrMediaSettings
	: public UObject
{
	GENERATED_BODY()

public:
	 
	/** Default constructor. */
	UExrMediaSettings();
};
