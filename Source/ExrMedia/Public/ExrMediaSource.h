// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Classes/Engine/EngineTypes.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "BaseMediaSource.h"

#include "ExrMediaSource.generated.h"


/**
 * Media source for EXR image sequences.
 */
UCLASS(BlueprintType, hidecategories=(Overrides, Playback))
class EXRMEDIA_API UExrMediaSource
	: public UBaseMediaSource
{
	GENERATED_BODY()

public:

	/** Default constructor. */
	UExrMediaSource();

public:

	/** Overrides the default frame rate stored in the EXR image files (0.0 = do not override). */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=EXR, AdvancedDisplay)
	float FramesPerSecondOverride;

public:

	/**
	 * Get the path to the EXR image sequence directory to be played.
	 *
	 * @return The file path.
	 * @see SetSourceDirectory
	 */
	const FString& GetSequencePath() const
	{
		return SequencePath.Path;
	}

	/**
	 * Set the path to the EXR image sequence directory this source represents.
	 *
	 * @param Path The path to set.
	 * @see SourceDirectory
	 */
	UFUNCTION(BlueprintCallable, Category="ExrMedia|ExrMediaSource")
	void SetSequencePath(const FString& Path);

public:

	//~ IMediaOptions interface

	virtual double GetMediaOption(const FName& Key, double DefaultValue) const override;
	virtual bool HasMediaOption(const FName& Key) const override;

public:

	//~ UMediaSource interface

	virtual FString GetUrl() const override;
	virtual bool Validate() const override;

protected:

	/** Get the full path to the image sequence. */
	FString GetFullPath() const;

protected:

	/** The directory that contains the EXR image sequence files. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=EXR)
	FDirectoryPath SequencePath;
};
