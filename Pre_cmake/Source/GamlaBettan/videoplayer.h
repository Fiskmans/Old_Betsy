#pragma once
#define USE_VIDEO

#ifdef USE_VIDEO
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include "FFmpeg/libavcodec/avcodec.h"
#include "FFmpeg/libavdevice/avdevice.h"
#include "FFmpeg/libavfilter/avfilter.h"
#include "FFmpeg/libavformat/avformat.h"
#include "FFmpeg/libavformat/avio.h"
#include "FFmpeg/libavutil/avutil.h"
#include "FFmpeg/libpostproc/postprocess.h"
#include "FFmpeg/libswresample/swresample.h"
#include "FFmpeg/libswscale/swscale.h"
#include <wtypes.h>

#ifdef __cplusplus
} // end extern "C".
#endif // __cplusplus

	class VideoPlayer
	{
	public:

		VideoPlayer();
		~VideoPlayer();

		bool Init(const char* aPath);
		bool DoFirstFrame();
		bool Update(int*& aBuffer);

		void Stop();
		int GrabNextFrame();
		void RestartStream();

		double GetFps();

		inline void SetIsDone(const bool aValue);
		inline bool GetIsDone() const;

		double myLeftOverTime;

		 volatile bool myIsDone;

		AVCodec*            l_pCodec;
		AVCodecContext*     l_pAVCodecContext;
		SwsContext*         l_pSWSContext;
		AVFormatContext*    l_pAVFormatContext;
		AVFrame*            l_pAVFrame;
		AVFrame*            l_pAVFrameBGR;
		AVPacket            l_AVPacket;
		std::string         l_sFile;
		uint8_t*            l_puiBuffer;
		int                 l_iResult;
		int                 l_iFrameCount = 0;
		int                 l_iGotFrame;
		int                 l_iDecodedBytes;
		int                 l_iVideoStreamIdx;
		int                 l_iNumBytes;
	//	AVCodecContext *video_dec_ctx;
	};
#endif

	void VideoPlayer::SetIsDone(const bool aValue)
	{
		myIsDone = aValue;
	}

	bool VideoPlayer::GetIsDone() const
	{
		return myIsDone;
	}