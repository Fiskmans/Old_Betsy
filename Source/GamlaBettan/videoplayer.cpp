#include "pch.h"
#include "videoplayer.h"


#ifdef USE_VIDEO

VideoPlayer::VideoPlayer()
{
	l_pCodec = NULL;
	l_pAVCodecContext = NULL;
	l_pSWSContext = NULL;
	l_pAVFormatContext = NULL;
	l_pAVFrame = NULL;
	l_pAVFrameBGR = NULL;
	l_puiBuffer = NULL;

	myLeftOverTime = 0.0;
	myIsDone = false;
}

VideoPlayer::~VideoPlayer()
{
	if (l_puiBuffer)
	{
		av_free(l_puiBuffer);
		l_puiBuffer = NULL;
	}
	// Free the RGB image
	if (l_pAVFrameBGR)
	{
		av_free(l_pAVFrameBGR);
		l_pAVFrameBGR = NULL;
	}
	
	// Free the YUV frame
	if (l_pAVFrame)
	{
		av_free(l_pAVFrame);
		l_pAVFrame = NULL;
	}
	
	// Close the codecs
	if (l_pAVCodecContext)
	{
		avcodec_close(l_pAVCodecContext);
		l_pAVCodecContext = NULL;
	}

	// Close the video file
	if (l_pAVFormatContext)
	{
		avformat_close_input(&l_pAVFormatContext);
		l_pAVFormatContext = NULL;
	}
	
	
	if (l_pSWSContext)
	{
		sws_freeContext(l_pSWSContext);
		l_pSWSContext = NULL;
	}
}

bool VideoPlayer::DoFirstFrame()
{
	GrabNextFrame();
	return true;
}

void VideoPlayer::Stop()
{
	av_seek_frame(l_pAVFormatContext, l_iVideoStreamIdx, 0, AVSEEK_FLAG_ANY);
}

double r2d(AVRational r)
{
	return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

#define CALC_FFMPEG_VERSION(a,b,c) ( a<<16 | b<<8 | c )
double get_fps(AVFormatContext* aContext, int aStream)
{
	double eps_zero = 0.000025;
	double fps = r2d(aContext->streams[aStream]->r_frame_rate);

#if LIBAVFORMAT_BUILD >= CALC_FFMPEG_VERSION(52, 111, 0)
	if (fps < eps_zero)
	{
		fps = r2d(aContext->streams[aStream]->avg_frame_rate);
	}
#endif

	if (fps < eps_zero)
	{
		fps = 1.0 / r2d(aContext->streams[aStream]->codec->time_base);
	}

	return fps;
}

void VideoPlayer::RestartStream()
{
	av_seek_frame(l_pAVFormatContext, l_iVideoStreamIdx, 0, AVSEEK_FLAG_ANY);
}

double VideoPlayer::GetFps()
{
	if (!l_pAVFormatContext)
	{
		return 0;
	}
	return get_fps(l_pAVFormatContext, l_iVideoStreamIdx);
}

bool VideoPlayer::Init(const char* aPath)
{	
	l_sFile = std::string(aPath);

	av_register_all();
	
	l_iResult = avformat_open_input(&l_pAVFormatContext,
		l_sFile.c_str(),
		NULL,
		NULL);

	if (l_iResult >= 0)
	{
		l_iResult = avformat_find_stream_info(l_pAVFormatContext, NULL);

		if (l_iResult >= 0)
		{
			for (unsigned int i = 0; i < l_pAVFormatContext->nb_streams; i++)
			{
				if (l_pAVFormatContext->streams[i]->codec->codec_type ==
					AVMEDIA_TYPE_VIDEO)
				{
					l_iVideoStreamIdx = i;

					l_pAVCodecContext =
						l_pAVFormatContext->streams[l_iVideoStreamIdx]->codec;

					if (l_pAVCodecContext)
					{
						l_pCodec = avcodec_find_decoder(l_pAVCodecContext->codec_id);
					}

					break;
				}
			}
		}
	}
	else
	{
		return false;
	}

	if (l_pCodec && l_pAVCodecContext)
	{
		l_iResult = avcodec_open2(l_pAVCodecContext, l_pCodec, NULL);

		if (l_iResult >= 0)
		{	
			l_pAVFrame = avcodec_alloc_frame();
			l_pAVFrameBGR = avcodec_alloc_frame();		

			AVPixelFormat format = AV_PIX_FMT_RGBA;

			l_iNumBytes = avpicture_get_size(format,
				l_pAVCodecContext->width,
				l_pAVCodecContext->height);

			l_puiBuffer = (uint8_t *)av_malloc(l_iNumBytes * sizeof(uint8_t));		

			avpicture_fill((AVPicture *)l_pAVFrameBGR,
				l_puiBuffer,
				format,
				l_pAVCodecContext->width,
				l_pAVCodecContext->height);
			
			l_pSWSContext = sws_getContext(
				l_pAVCodecContext->width,
				l_pAVCodecContext->height,
				l_pAVCodecContext->pix_fmt,
				l_pAVCodecContext->width,
				l_pAVCodecContext->height,
				format,
				SWS_BILINEAR,
				NULL,
				NULL,
				NULL);
		}
	}
	else
	{
		return false;
	}

	return true;
}

int VideoPlayer::GrabNextFrame()
{
	bool valid = false;
	int readFrame = 0;
	int safeValue = 0;
	const int maxSafeVal = 10000;

	while (!valid || safeValue > maxSafeVal)
	{
		safeValue++;
		readFrame = av_read_frame(l_pAVFormatContext, &l_AVPacket);
		if (readFrame >= 0)
		{
			if (l_AVPacket.stream_index == l_iVideoStreamIdx)
			{
				l_iDecodedBytes = avcodec_decode_video2(
					l_pAVCodecContext,
					l_pAVFrame,
					&l_iGotFrame,
					&l_AVPacket);

				if (l_iGotFrame)
				{
					valid = true;
				}
			}
		}
		else
		{
			valid = true;
		}
		av_free_packet(&l_AVPacket);
	}

	if (safeValue >= maxSafeVal)
	{
		//ERROR
	}

	return readFrame;
}

bool VideoPlayer::Update(int*& aBuffer)
{
	if (!l_iGotFrame)
	{
		return false;
	}

	if (l_pSWSContext)
	{
		l_iResult = sws_scale(
			l_pSWSContext,
			l_pAVFrame->data,
			l_pAVFrame->linesize,
			0,
			l_pAVCodecContext->height,
			l_pAVFrameBGR->data,
			l_pAVFrameBGR->linesize);

		if (l_iResult > 0)
		{
			memcpy(aBuffer, l_pAVFrameBGR->data[0], l_iNumBytes);

			av_free_packet(&l_AVPacket);
		}
	}

	l_iFrameCount++;

	return true;
}
#endif