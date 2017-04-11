// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"
#include "UObject/NameTypes.h"

#include "../../ExrMediaFactory/Public/ExrMediaSettings.h"


/** Log category for the this module. */
DECLARE_LOG_CATEGORY_EXTERN(LogExrMedia, Log, All);


namespace ExrMedia
{
	/** Name of the FramesPerSecondAttribute media option. */
	static FName FramesPerSecondAttributeOption("FramesPerSecondAttribute");

	/** Name of the FramesPerSecondOverride media option. */
	static FName FramesPerSecondOverrideOption("FramesPerSecondOverride");
}
