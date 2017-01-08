// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;
class IPropertyHandle;


/**
 * Implements a details view customization for the UExrMediaSource class.
 */
class FExrMediaSourceCustomization
	: public IDetailCustomization
{
public:

	//~ IDetailCustomization interface

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

public:

	/**
	 * Creates an instance of this class.
	 *
	 * @return The new instance.
	 */
	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShareable(new FExrMediaSourceCustomization());
	}

private:

	/** Callback for getting the selected path in the SequencePath picker widget. */
	FString HandleSequencePathPickerFilePath() const;

	/** Callback for getting the file type filter for the SequencePath picker. */
	FString HandleSequencePathPickerFileTypeFilter() const;

	/** Callback for picking a path in the source directory picker. */
	void HandleSequencePathPickerPathPicked(const FString& PickedPath);

	/** Callback for getting the visibility of warning icon for invalid SequencePath paths. */
	EVisibility HandleSequencePathWarningIconVisibility() const;

private:

	/** Pointer to the SequencePath.Path property handle. */
	TSharedPtr<IPropertyHandle> SequencePathProperty;
};
