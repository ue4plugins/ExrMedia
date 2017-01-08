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

void UExrMediaSource::SetSequencePath(const FString& Path)
{
	const FString SanitizedPath = FPaths::GetPath(Path);

	if (SanitizedPath.IsEmpty() || SanitizedPath.StartsWith(TEXT(".")))
	{
		SequencePath.Path = SanitizedPath;
	}
	else
	{
		FString FullPath = FPaths::ConvertRelativePathToFull(SanitizedPath);
		const FString FullGameContentDir = FPaths::ConvertRelativePathToFull(FPaths::GameContentDir());

		if (FullPath.StartsWith(FullGameContentDir))
		{
			FPaths::MakePathRelativeTo(FullPath, *FullGameContentDir);
			FullPath = FString(TEXT("./")) + FullPath;
		}

		SequencePath.Path = FullPath;
	}
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
	return FString(TEXT("exr://")) + GetFullPath();
}


bool UExrMediaSource::Validate() const
{
	return FPaths::DirectoryExists(GetFullPath());
}


/* UFileMediaSource implementation
 *****************************************************************************/

FString UExrMediaSource::GetFullPath() const
{
	if (!FPaths::IsRelative(SequencePath.Path))
	{
		return SequencePath.Path;
	}

	if (SequencePath.Path.StartsWith(TEXT("./")))
	{
		return FPaths::ConvertRelativePathToFull(FPaths::GameContentDir(), SequencePath.Path.RightChop(2));
	}

	return FPaths::ConvertRelativePathToFull(SequencePath.Path);
}
