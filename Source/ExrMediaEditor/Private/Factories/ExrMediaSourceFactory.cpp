// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ExrMediaSourceFactory.h"
#include "Misc/Paths.h"
#include "ExrMediaSource.h"


/* UExrFileMediaSourceFactory structors
 *****************************************************************************/

UExrMediaSourceFactory::UExrMediaSourceFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Formats.Add(TEXT("exr;EXR Image Sequence"));

	SupportedClass = UExrMediaSource::StaticClass();
	bEditorImport = true;
}


/* UFactory overrides
 *****************************************************************************/

UObject* UExrMediaSourceFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	UExrMediaSource* MediaSource = NewObject<UExrMediaSource>(InParent, InClass, InName, Flags);
	MediaSource->SetSequencePath(FPaths::GetPath(CurrentFilename));

	return MediaSource;
}
