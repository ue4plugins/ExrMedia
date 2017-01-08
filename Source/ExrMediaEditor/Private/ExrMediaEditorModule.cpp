// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

#include "ExrMediaSource.h"

#include "Customizations/ExrMediaSourceCustomization.h"


/**
 * Implements the ExrMediaEditor module.
 */
class FExrMediaEditorModule
	: public IModuleInterface
{
public:

	//~ IModuleInterface interface

	virtual void StartupModule() override
	{
		RegisterCustomizations();
	}

	virtual void ShutdownModule() override
	{
		UnregisterCustomizations();
	}

protected:

	/** Register details view customizations. */
	void RegisterCustomizations()
	{
		ExrMediaSourceName = UExrMediaSource::StaticClass()->GetFName();

		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		{
			PropertyModule.RegisterCustomClassLayout(ExrMediaSourceName, FOnGetDetailCustomizationInstance::CreateStatic(&FExrMediaSourceCustomization::MakeInstance));
		}
	}

	/** Unregister details view customizations. */
	void UnregisterCustomizations()
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		{
			PropertyModule.UnregisterCustomClassLayout(ExrMediaSourceName);
		}
	}

private:

	/** Class names. */
	FName ExrMediaSourceName;
};


IMPLEMENT_MODULE(FExrMediaEditorModule, ExrMediaEditor);
