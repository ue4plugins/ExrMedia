// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ExrMediaSource.h"
#include "ExrMediaPrivate.h"

#include "Misc/Paths.h"


/* UExrMediaSource structors
 *****************************************************************************/

UExrMediaSource::UExrMediaSource()
	: FramesPerSecondOverride(0.0f)
{ }


/* UExrMediaSource interface
 *****************************************************************************/

void UExrMediaSource::SetSourceDirectory(const FString& Path)
{
	SourceDirectory.Path = FPaths::GetPath(Path);
}


/* IMediaOptions interface
 *****************************************************************************/

double UExrMediaSource::GetMediaOption(const FName& Key, const double DefaultValue) const
{
	if (Key == ExrMedia::FramesPerSecondOverrideOption)
	{
		return FramesPerSecondOverride;
	}

	return Super::GetMediaOption(Key, DefaultValue);
}


bool UExrMediaSource::HasMediaOption(const FName& Key) const
{
	if (Key == ExrMedia::FramesPerSecondOverrideOption)
	{
		return true;
	}

	return Super::HasMediaOption(Key);
}


/* UMediaSource interface
 *****************************************************************************/

FString UExrMediaSource::GetUrl() const
{
	return FString(TEXT("exr://")) + SourceDirectory.Path;
}


bool UExrMediaSource::Validate() const
{
	return FPaths::DirectoryExists(SourceDirectory.Path);
}
