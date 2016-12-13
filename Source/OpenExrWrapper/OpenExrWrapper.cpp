#pragma warning(push)
#pragma warning(disable:28251)

#include "OpenExrWrapper.h"

#include "ImathBox.h"
#include "ImfHeader.h"
#include "ImfRgbaFile.h"
#include "ImfStandardAttributes.h"
#include "Modules/ModuleManager.h"


FRgbaInputFile::FRgbaInputFile(const FString& FilePath)
{
	InputFile = new Imf::RgbaInputFile(TCHAR_TO_ANSI(*FilePath));
}


FRgbaInputFile::~FRgbaInputFile()
{
	delete (Imf::RgbaInputFile*)InputFile;
}


FIntPoint FRgbaInputFile::GetDataWindow() const
{
	Imath::Box2i Win = ((Imf::RgbaInputFile*)InputFile)->dataWindow();

	return FIntPoint(
		Win.max.x - Win.min.x + 1,
		Win.max.y - Win.min.y + 1
	);
}


double FRgbaInputFile::GetFramesPerSecond(double DefaultValue) const
{
	auto Attribute = ((Imf::RgbaInputFile*)InputFile)->header().findTypedAttribute<Imf::RationalAttribute>("framesPerSecond");

	if (Attribute == nullptr)
	{
		return DefaultValue;
	}

	return Attribute->value();
}


void FRgbaInputFile::ReadPixels(int32 StartY, int32 EndY)
{
	Imath::Box2i Win = ((Imf::RgbaInputFile*)InputFile)->dataWindow();
	((Imf::RgbaInputFile*)InputFile)->readPixels(Win.min.y, Win.max.y);
}


void FRgbaInputFile::SetFrameBuffer(void* Buffer, const FIntPoint& BufferDim)
{
	Imath::Box2i Win = ((Imf::RgbaInputFile*)InputFile)->dataWindow();
	((Imf::RgbaInputFile*)InputFile)->setFrameBuffer((Imf::Rgba*)Buffer - Win.min.x - Win.min.y * BufferDim.X, 1, BufferDim.X);
}


IMPLEMENT_MODULE(FDefaultModuleImpl, OpenExrWrapper);

#pragma warning(pop)
