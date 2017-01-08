// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Customizations/ExrMediaSourceCustomization.h"
#include "Misc/Paths.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SBoxPanel.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorStyleSet.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Input/SDirectoryPicker.h"
#include "Widgets/Input/SFilePathPicker.h"
#include "Widgets/Images/SImage.h"
#include "DetailWidgetRow.h"
#include "IDetailPropertyRow.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "IMediaModule.h"


#define LOCTEXT_NAMESPACE "FExrMediaSourceCustomization"


/* IDetailCustomization interface
 *****************************************************************************/

void FExrMediaSourceCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// customize 'File' category
	IDetailCategoryBuilder& FileCategory = DetailBuilder.EditCategory("EXR");
	{
		// FilePath
		SequencePathProperty = DetailBuilder.GetProperty("SequencePath");
		{
			IDetailPropertyRow& SequencePathRow = FileCategory.AddProperty(SequencePathProperty);

			SequencePathRow
				.ShowPropertyButtons(false)
				.CustomWidget()
				.NameContent()
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
							.AutoWidth()
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
									.Font(IDetailLayoutBuilder::GetDetailFont())
									.Text(LOCTEXT("SequencePathPropertyName", "Sequence Path"))
									.ToolTipText(SequencePathProperty->GetToolTipText())
							]

						+ SHorizontalBox::Slot()
							.FillWidth(1.0f)
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Center)
							.Padding(4.0f, 0.0f, 0.0f, 0.0f)
							[
								SNew(SImage)
									.Image(FCoreStyle::Get().GetBrush("Icons.Warning"))
									.ToolTipText(LOCTEXT("SequencePathWarning", "The selected image sequence will not get packaged, because its path points to a directory outside the project's /Content/Movies/ directory."))
									.Visibility(this, &FExrMediaSourceCustomization::HandleSequencePathWarningIconVisibility)
							]
					]
				.ValueContent()
					.MaxDesiredWidth(0.0f)
					.MinDesiredWidth(125.0f)
					[
						SNew(SFilePathPicker)
							.BrowseButtonImage(FEditorStyle::GetBrush("PropertyWindow.Button_Ellipsis"))
							.BrowseButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
							.BrowseButtonToolTip(LOCTEXT("SequencePathBrowseButtonToolTip", "Choose a file from this computer"))
							.BrowseDirectory(FPaths::GameContentDir() / TEXT("Movies"))
							.FilePath(this, &FExrMediaSourceCustomization::HandleSequencePathPickerFilePath)
							.FileTypeFilter(this, &FExrMediaSourceCustomization::HandleSequencePathPickerFileTypeFilter)
							.OnPathPicked(this, &FExrMediaSourceCustomization::HandleSequencePathPickerPathPicked)
							.ToolTipText(LOCTEXT("SequencePathToolTip", "The path to an image sequence file on this computer"))
					];
		}
	}
}


/* FExrMediaSourceCustomization callbacks
 *****************************************************************************/

FString FExrMediaSourceCustomization::HandleSequencePathPickerFilePath() const
{
	FString FilePath;
	SequencePathProperty->GetChildHandle("Path")->GetValue(FilePath);

	return FilePath;
}


FString FExrMediaSourceCustomization::HandleSequencePathPickerFileTypeFilter() const
{
	FString Filter = TEXT("All files (*.*)|*.*");
	Filter += TEXT("|EXR files (*.exr)|*.exr");

	return Filter;
}


void FExrMediaSourceCustomization::HandleSequencePathPickerPathPicked(const FString& PickedPath)
{
	TSharedPtr<IPropertyHandle> SequencePathPathProperty = SequencePathProperty->GetChildHandle("Path");
	const FString SanitizedPath = FPaths::GetPath(PickedPath);

	if (SanitizedPath.IsEmpty() || SanitizedPath.StartsWith(TEXT("./")))
	{
		SequencePathPathProperty->SetValue(SanitizedPath);
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

		SequencePathPathProperty->SetValue(FullPath);
	}
}


EVisibility FExrMediaSourceCustomization::HandleSequencePathWarningIconVisibility() const
{
	FString FilePath;

	if ((SequencePathProperty->GetValue(FilePath) != FPropertyAccess::Success) || FilePath.IsEmpty() || FilePath.Contains(TEXT("://")))
	{
		return EVisibility::Hidden;
	}

	const FString FullMoviesPath = FPaths::ConvertRelativePathToFull(FPaths::GameContentDir() / TEXT("Movies"));
	const FString FullPath = FPaths::ConvertRelativePathToFull(FPaths::IsRelative(FilePath) ? FPaths::GameContentDir() / FilePath : FilePath);

	if (FullPath.StartsWith(FullMoviesPath))
	{
		if (FPaths::FileExists(FullPath))
		{
			return EVisibility::Hidden;
		}

		// file doesn't exist
		return EVisibility::Visible;
	}

	// file not inside Movies folder
	return EVisibility::Visible;
}


#undef LOCTEXT_NAMESPACE
