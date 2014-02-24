#ifndef _SOUND_CLASS_H_
#define _SOUND_CLASS_H_

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Class name: SoundClass
///////////////////////////////////////////////////////////////////////////////
class SoundClass
{
	private:
		struct WaveHeaderType
		{
			char chunkId[4];
			unsigned long chunkSize;
			char format[4];
			char subChunkId[4];
			unsigned long subChunkSize;
			unsigned short audioFormat;
			unsigned short numChannels;
			unsigned long sampleRate;
			unsigned long bytesPerSecond;
			unsigned short blockAlign;
			unsigned short bitsPerSample;
			char dataChunkId[4];
			unsigned long dataSize;
		};

	public:
		SoundClass();
		SoundClass(const SoundClass&);
		~SoundClass();

		bool setup(HWND, std::string fileName);
		void destroy();

		bool playHiFile();
		bool playEatingFile();
		bool playSurpriseFile();
		bool playCelebratingFile();

	private:
		bool setupDirectSound(HWND);
		void destroyDirectSound();

		bool loadWaveFile(char*, IDirectSoundBuffer8**);
		void destroyWaveFile(IDirectSoundBuffer8**);

	private:
		IDirectSound8* m_DirectSound;
		IDirectSoundBuffer* m_primaryBuffer;
		IDirectSoundBuffer8* eatingBuffer_;
		IDirectSoundBuffer8* surpriseBuffer_;
		IDirectSoundBuffer8* celebratingBuffer_;
		IDirectSoundBuffer8* hiBuffer_;
};

#endif //_SOUND_CLASS_H_