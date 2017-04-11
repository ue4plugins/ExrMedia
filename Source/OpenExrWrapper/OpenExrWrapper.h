// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Math/IntPoint.h"

class FString;


class OPENEXRWRAPPER_API FRgbaInputFile
{
public:

	FRgbaInputFile(const FString& FilePath);
	~FRgbaInputFile();

public:

	FIntPoint GetDataWindow() const;
	double GetFramesPerSecond(double DefaultValue) const;
	void ReadPixels(int32 StartY, int32 EndY);
	void SetFrameBuffer(void* Buffer, const FIntPoint& Stride);

private:

	void* InputFile;
};
