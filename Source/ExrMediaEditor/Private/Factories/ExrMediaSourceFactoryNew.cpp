// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ExrMediaSourceFactoryNew.h"

#include "AssetTypeCategories.h"
#include "ExrMediaSource.h"


/* UExrMediaSourceFactoryNew structors
 *****************************************************************************/

UExrMediaSourceFactoryNew::UExrMediaSourceFactoryNew(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UExrMediaSource::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}


/* UFactory overrides
 *****************************************************************************/

UObject* UExrMediaSourceFactoryNew::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UExrMediaSource>(InParent, InClass, InName, Flags);
}


uint32 UExrMediaSourceFactoryNew::GetMenuCategories() const
{
	return EAssetTypeCategories::Media;
}


bool UExrMediaSourceFactoryNew::ShouldShowInNewMenu() const
{
	return true;
}
