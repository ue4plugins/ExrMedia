// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ExrMediaPlayer.h"
#include "ExrMediaPrivate.h"

#include "HAL/FileManager.h"
#include "IMediaOptions.h"
#include "IMediaTextureSink.h"
#include "Misc/Paths.h"
#include "Misc/ScopeLock.h"
#include "OpenExrWrapper.h"


#define LOCTEXT_NAMESPACE "FExrMediaPlayer"


/* FExrVideoPlayer structors
 *****************************************************************************/

FExrMediaPlayer::FExrMediaPlayer()
	: CurrentDim(FIntPoint::ZeroValue)
	, CurrentFps(0.0)
	, CurrentTime(0.0f)
	, Duration(0.0f)
	, LastFrameIndex(INDEX_NONE)
	, SelectedVideoTrack(INDEX_NONE)
	, VideoSink(nullptr)
{ }


FExrMediaPlayer::~FExrMediaPlayer()
{
	Close();
}


/* IMediaControls interface
 *****************************************************************************/

FTimespan FExrMediaPlayer::GetDuration() const
{
	return FTimespan::FromSeconds(Duration);
}


float FExrMediaPlayer::GetRate() const
{
	if (CurrentUrl.IsEmpty())
	{
		return 0.0f;
	}

	return CurrentRate;
}


EMediaState FExrMediaPlayer::GetState() const
{
	if (CurrentUrl.IsEmpty())
	{
		return EMediaState::Closed;
	}

	return (CurrentRate == 0.0f) ? EMediaState::Paused : EMediaState::Playing;
}


TRange<float> FExrMediaPlayer::GetSupportedRates(EMediaPlaybackDirections Direction, bool Unthinned) const
{
	return TRange<float>(0.0f, 100000.0f);
}


FTimespan FExrMediaPlayer::GetTime() const
{
	return FTimespan::FromSeconds(CurrentTime);
}


bool FExrMediaPlayer::IsLooping() const
{
	return ShouldLoop;
}


bool FExrMediaPlayer::Seek(const FTimespan& Time)
{
	if (CurrentUrl.IsEmpty())
	{
		return false;
	}

	CurrentTime = Time.GetTotalSeconds();

	return true;
}


bool FExrMediaPlayer::SetLooping(bool Looping)
{
	ShouldLoop = Looping;

	return true;
}


bool FExrMediaPlayer::SetRate(float Rate)
{
	if (CurrentUrl.IsEmpty())
	{
		return false;
	}

	CurrentRate = Rate;

	return true;
}


bool FExrMediaPlayer::SupportsRate(float Rate, bool Unthinned) const
{
	return true;
}


bool FExrMediaPlayer::SupportsScrubbing() const
{
	return true;
}


bool FExrMediaPlayer::SupportsSeeking() const
{
	return true;
}


/* IMediaPlayer interface
 *****************************************************************************/

void FExrMediaPlayer::Close()
{
	{
		FScopeLock Lock(&CriticalSection);

		CurrentDim = FIntPoint::ZeroValue;
		CurrentFps = 0.0f;
		CurrentTime = 0.0f;
		CurrentUrl.Empty();
		Duration = 0.0f;
		ImagePaths.Empty();
		Info.Empty();
		LastFrameIndex = INDEX_NONE;
		SelectedVideoTrack = INDEX_NONE;
	}

	MediaEvent.Broadcast(EMediaEvent::TracksChanged);
	MediaEvent.Broadcast(EMediaEvent::MediaClosed);
}


IMediaControls& FExrMediaPlayer::GetControls()
{
	return *this;
}


FString FExrMediaPlayer::GetInfo() const
{
	return Info;
}


FName FExrMediaPlayer::GetName() const
{
	static FName PlayerName(TEXT("ExrMedia"));
	return PlayerName;
}


IMediaOutput& FExrMediaPlayer::GetOutput()
{
	return *this;
}


FString FExrMediaPlayer::GetStats() const
{
	FString StatsString;
	{
		StatsString += TEXT("not implemented yet");
		StatsString += TEXT("\n");
	}

	return StatsString;
}


IMediaTracks& FExrMediaPlayer::GetTracks()
{
	return *this;
}


FString FExrMediaPlayer::GetUrl() const
{
	return CurrentUrl;
}


bool FExrMediaPlayer::Open(const FString& Url, const IMediaOptions& Options)
{
	Close();

	if (Url.IsEmpty() || !Url.StartsWith(TEXT("exr://")))
	{
		return false;
	}

	const TCHAR* SequencePath = &Url[6];

	// locate image sequence files
	TArray<FString> OutImageFiles;
	{
		IFileManager::Get().FindFiles(OutImageFiles, SequencePath, TEXT("*.exr"));

		if (OutImageFiles.Num() == 0)
		{
			UE_LOG(LogExrMedia, Error, TEXT("The directory %s does not contain any .exr image files"), SequencePath);
			return false;
		}

		UE_LOG(LogExrMedia, Verbose, TEXT("Found %i EXR image files in %s"), SequencePath);

		OutImageFiles.Sort();
	}

	// fetch sequence attributes from first image
	FRgbaInputFile InputFile(FPaths::Combine(SequencePath, OutImageFiles[0]));
	FIntPoint Dim = InputFile.GetDataWindow();

	if (Dim.GetMin() <= 0)
	{
		UE_LOG(LogExrMedia, Error, TEXT("The image sequence does not contain a valid data window size"));
		return false;
	}

	double Fps = Options.GetMediaOption(ExrMedia::FramesPerSecondOverrideOption, 0.0f);

	if (Fps == 0.0f)
	{
		Fps = InputFile.GetFramesPerSecond(24.0);
	}

	// finalize initialization
	{
		FScopeLock Lock(&CriticalSection);

		for (const auto& ImageFile : OutImageFiles)
		{
			ImagePaths.Add(FPaths::Combine(SequencePath, ImageFile));
		}

		CurrentDim = Dim;
		CurrentFps = Fps;
		CurrentUrl = Url;
		Duration = ImagePaths.Num() / Fps;
	}

	Info += TEXT("Image Sequence\n");
	Info += FString::Printf(TEXT("    Dimension: %i x %i\n"), CurrentDim.X, CurrentDim.Y);
	Info += FString::Printf(TEXT("    Frames: %i\n"), ImagePaths.Num());
	Info += FString::Printf(TEXT("    FPS: %f\n"), CurrentFps);

	// notify listeners
	MediaEvent.Broadcast(EMediaEvent::TracksChanged);
	MediaEvent.Broadcast(EMediaEvent::MediaOpened);

	return true;
}


bool FExrMediaPlayer::Open(const TSharedRef<FArchive, ESPMode::ThreadSafe>& Archive, const FString& OriginalUrl, const IMediaOptions& Options)
{
	return false; // not supported
}


void FExrMediaPlayer::TickPlayer(float DeltaTime)
{
	// do nothing
}


void FExrMediaPlayer::TickVideo(float DeltaTime)
{
	if (Duration == 0.0f)
	{
		return;
	}

	// update clock
	CurrentTime += DeltaTime * CurrentRate;
	CurrentTime = FMath::Fmod(CurrentTime, Duration);

	FScopeLock Lock(&CriticalSection);

	if ((ImagePaths.Num() == 0) || (VideoSink == nullptr))
	{
		return;
	}

	// skip frame if already processed
	const int32 FrameIndex = (int)(CurrentTime * CurrentFps);

	if (FrameIndex == LastFrameIndex)
	{
		return;
	}

	LastFrameIndex = FrameIndex;

	// fetch frame
	const FString ImagePath = ImagePaths[FrameIndex];

	FRgbaInputFile InputFile(ImagePath);
	FIntPoint Dim = InputFile.GetDataWindow();

	if (Dim != CurrentDim)
	{
		UE_LOG(LogExrMedia, Warning, TEXT("Image frame %s is %s instead of %s"), *ImagePath, *Dim.ToString(), *CurrentDim.ToString());
	}

	// re-initialize sink if format changed
	if (VideoSink->GetTextureSinkDimensions() != Dim)
	{
		if (!VideoSink->InitializeTextureSink(Dim, Dim, EMediaTextureSinkFormat::FloatRGBA, EMediaTextureSinkMode::Unbuffered))
		{
			return;
		}
	}

	// read frame data
	void* TextureBuffer = VideoSink->AcquireTextureSinkBuffer();

	if (TextureBuffer != nullptr)
	{
		InputFile.SetFrameBuffer(TextureBuffer, CurrentDim);
		InputFile.ReadPixels(0, CurrentDim.Y - 1);

		VideoSink->ReleaseTextureSinkBuffer();
		VideoSink->DisplayTextureSinkBuffer(FTimespan::FromSeconds(CurrentTime));
	}
}


/* IMediaOutput interface
 *****************************************************************************/

void FExrMediaPlayer::SetAudioSink(IMediaAudioSink* Sink)
{
	// not supported
}


void FExrMediaPlayer::SetMetadataSink(IMediaBinarySink* Sink)
{
	// not supported
}


void FExrMediaPlayer::SetOverlaySink(IMediaOverlaySink* Sink)
{
	// not supported
}


void FExrMediaPlayer::SetVideoSink(IMediaTextureSink* Sink)
{
	if (Sink == VideoSink)
	{
		return;
	}

	FScopeLock Lock(&CriticalSection);

	if (VideoSink != nullptr)
	{
		VideoSink->ShutdownTextureSink();
	}

	VideoSink = Sink;

	if (Sink != nullptr)
	{
		Sink->InitializeTextureSink(CurrentDim, CurrentDim, EMediaTextureSinkFormat::FloatRGBA, EMediaTextureSinkMode::Unbuffered);
	}
}


/* IMediaTracks interface
 *****************************************************************************/

uint32 FExrMediaPlayer::GetAudioTrackChannels(int32 TrackIndex) const
{
	return 0; // not supported
}


uint32 FExrMediaPlayer::GetAudioTrackSampleRate(int32 TrackIndex) const
{
	return 0; // not supported
}


int32 FExrMediaPlayer::GetNumTracks(EMediaTrackType TrackType) const
{
	if (CurrentUrl.IsEmpty() || (TrackType != EMediaTrackType::Video))
	{
		return 0;
	}

	return 1;
}


int32 FExrMediaPlayer::GetSelectedTrack(EMediaTrackType TrackType) const
{
	if (CurrentUrl.IsEmpty() || (TrackType != EMediaTrackType::Video))
	{
		return INDEX_NONE;
	}

	return SelectedVideoTrack;
}


FText FExrMediaPlayer::GetTrackDisplayName(EMediaTrackType TrackType, int32 TrackIndex) const
{
	if (CurrentUrl.IsEmpty() || (TrackType != EMediaTrackType::Video) || (TrackIndex != 0))
	{
		return FText::GetEmpty();
	}

	return LOCTEXT("DefaultVideoTrackName", "Video Track");
}


FString FExrMediaPlayer::GetTrackLanguage(EMediaTrackType TrackType, int32 TrackIndex) const
{
	if (CurrentUrl.IsEmpty() || (TrackType != EMediaTrackType::Video) || (TrackIndex != 0))
	{
		return FString();
	}

	return TEXT("und");
}


FString FExrMediaPlayer::GetTrackName(EMediaTrackType TrackType, int32 TrackIndex) const
{
	if (CurrentUrl.IsEmpty() || (TrackType != EMediaTrackType::Video) || (TrackIndex != 0))
	{
		return FString();
	}

	return TEXT("VideoTrack");
}


uint32 FExrMediaPlayer::GetVideoTrackBitRate(int32 TrackIndex) const
{
	if (CurrentUrl.IsEmpty() || (TrackIndex != 0))
	{
		return 0;
	}

	return CurrentDim.X * CurrentDim.Y * sizeof(float);
}


FIntPoint FExrMediaPlayer::GetVideoTrackDimensions(int32 TrackIndex) const
{
	if (CurrentUrl.IsEmpty() || (TrackIndex != 0))
	{
		return FIntPoint::ZeroValue;
	}

	return CurrentDim;
}


float FExrMediaPlayer::GetVideoTrackFrameRate(int32 TrackIndex) const
{
	if (CurrentUrl.IsEmpty() || (TrackIndex != 0))
	{
		return 0;
	}

	return CurrentFps;
}


bool FExrMediaPlayer::SelectTrack(EMediaTrackType TrackType, int32 TrackIndex)
{
	if (CurrentUrl.IsEmpty() || (TrackType != EMediaTrackType::Video) || (TrackIndex != 0))
	{
		return false;
	}

	SelectedVideoTrack = TrackIndex;

	return true;
}
