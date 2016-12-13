// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ExrMediaPrivate.h"

#include "ExrMediaPlayer.h"
#include "IExrMediaModule.h"
#include "Modules/ModuleManager.h"


DEFINE_LOG_CATEGORY(LogExrMedia);

#define LOCTEXT_NAMESPACE "FExrMediaModule"


/**
 * Implements the AVFMedia module.
 */
class FExrMediaModule
	: public IExrMediaModule
{
public:

	/** Default constructor. */
	FExrMediaModule() { }

public:

	//~ IExrMediaModule interface

	virtual TSharedPtr<IMediaPlayer> CreatePlayer() override
	{
		return MakeShareable(new FExrMediaPlayer());
	}

public:

	//~ IModuleInterface interface

	virtual void StartupModule() override { }
	virtual void ShutdownModule() override { }
};


IMPLEMENT_MODULE(FExrMediaModule, ExrMedia);


#undef LOCTEXT_NAMESPACE
