// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SharedPointer.h"
#include "Modules/ModuleInterface.h"

class IMediaPlayer;


/**
 * Interface for the ExrMedia module.
 */
class IExrMediaModule
	: public IModuleInterface
{
public:

	/**
	 * Creates a media player for EXR image sequences.
	 *
	 * @return A new media player, or nullptr if a player couldn't be created.
	 */
	virtual TSharedPtr<IMediaPlayer> CreatePlayer() = 0;

public:

	/** Virtual destructor. */
	virtual ~IExrMediaModule() { }
};
