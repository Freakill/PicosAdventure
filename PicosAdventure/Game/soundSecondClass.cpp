#include "soundSecondClass.h"

SoundSecondClass::SoundSecondClass()
{
	m_DirectSound = 0;
	m_primaryBuffer = 0;

	sounds_.clear();
}

SoundSecondClass::SoundSecondClass(const SoundSecondClass& other)
{

}

SoundSecondClass::~SoundSecondClass()
{

}

bool SoundSecondClass::setup(HWND hwnd)
{
	bool result;

	// Initialize direct sound and the primary sound buffer.
	result = setupDirectSound(hwnd);
	if(!result)
	{
		MessageBoxA(NULL, "Could not setup Direct Sound", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Load a wave audio file onto a secondary buffer.
	result = loadFile("celebracion_1", "Feliz_celebracion_1");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load celebracion_1", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadFile("celebracion_2", "Feliz_celebracion_2");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load celebracion_2", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadFile("hola_1", "Hola_1");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load hola_1", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadFile("hola_2", "Hola_2");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load hola_2", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadFile("fixing", "pico_fixing");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load fixing", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadFile("pointing_1", "pointing_1");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load pointing_1", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadFile("pointing_2", "pointing_2");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load pointing_2", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadFile("ronronear_1", "ronronear_1");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load ronronear_1", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadFile("ronronear_2", "ronronear_2");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load ronronear_2", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadFile("sorprendido_1", "Sorprendido_1");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load sorprendido_1", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadFile("sorprendido_2", "Sorprendido_2");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load sorprendido_2", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}


	// FOREST
	result = loadFile("background_music", "background_forest");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load background_music", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadFile("piece_fall", "fruit_fall");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load piece_fall", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadFile("sound_aid", "owl");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load sound_aid", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadFile("piece_spaceship", "piece_spaceship");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load piece_spaceship", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadFile("spaceship_finished", "spaceship_finished");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load spaceship_finished", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	result = loadFile("falling_star", "falling_star");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load falling_star", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	/*result = loadFile("fuel1", "fuel1");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load fuel1", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadFile("fuel2", "fuel2");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load fuel2", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadFile("fuel3", "fuel3");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load fuel3", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadFile("fuel4", "fuel4");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load fuel4", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}
	result = loadFile("fuel5", "fuel5");
	if(!result)
	{
		MessageBoxA(NULL, "Could not load fuel5", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}*/

	return true;
}

void SoundSecondClass::destroy()
{
	std::map<std::string, IDirectSoundBuffer8*>::iterator soundIt;
	for(soundIt = sounds_.begin(); soundIt != sounds_.end(); soundIt++)
	{
		destroyWaveFile(&(soundIt->second));
	}
	sounds_.clear();

	// Shutdown the Direct Sound API.
	destroyDirectSound();

	return;
}

bool SoundSecondClass::setupDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;


	// Initialize the direct sound interface pointer for the default sound device.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the primary buffer description.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the format of the primary sound bufffer.
	// In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format).
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the primary buffer to be the wave format specified.
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void SoundSecondClass::destroyDirectSound()
{
	// Release the primary sound buffer pointer.
	if(m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	// Release the direct sound interface pointer.
	if(m_DirectSound)
	{
		m_DirectSound->Release();
		m_DirectSound = 0;
	}

	return;
}

bool SoundSecondClass::loadFile(std::string name, std::string fileName)
{
	bool result;

	std::string root = "./Data/sounds/" + fileName + ".wav";
	IDirectSoundBuffer8* soundTemp;

	result = loadWaveFile(root, &soundTemp);
	if(!result)
	{
		MessageBoxA(NULL, "Could not load file.", "SoundSecondClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	sounds_.insert(std::pair<std::string, IDirectSoundBuffer8*>(name, soundTemp));

	return true;
}

bool SoundSecondClass::loadWaveFile(std::string filename, IDirectSoundBuffer8** secondaryBuffer)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;


	// Open the wave file in binary.
	error = fopen_s(&filePtr, filename.c_str(), "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the wave file header.
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Check that the chunk ID is the RIFF format.
	if((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || 
	   (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		return false;
	}

	// Check that the file format is the WAVE format.
	if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
	   (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		return false;
	}

	// Check that the sub chunk ID is the fmt format.
	if((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
	   (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		return false;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if(waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		return false;
	}

	// Check that the wave file was recorded in stereo format.
	if(waveFileHeader.numChannels != 2)
	{
		return false;
	}

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if(waveFileHeader.sampleRate != 44100)
	{
		return false;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if(waveFileHeader.bitsPerSample != 16)
	{
		return false;
	}

	// Check for the data chunk header.
	if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
	   (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		return false;
	}

	// Set the wave format of secondary buffer that this wave file will be loaded onto.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Create a temporary sound buffer with the specific buffer settings.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;

	// Move to the beginning of the wave data which starts at the end of the data chunk header.
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	// Create a temporary buffer to hold the wave file data.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if(!waveData)
	{
		return false;
	}

	// Read in the wave file data into the newly created buffer.
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if(count != waveFileHeader.dataSize)
	{
		return false;
	}

	// Close the file once done reading.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Lock the secondary buffer to write wave data into it.
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if(FAILED(result))
	{
		return false;
	}

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if(FAILED(result))
	{
		return false;
	}
	
	// Release the wave data since it was copied into the secondary buffer.
	delete [] waveData;
	waveData = 0;

	return true;
}


void SoundSecondClass::destroyWaveFile(IDirectSoundBuffer8** secondaryBuffer)
{
	// Release the secondary sound buffer.
	if(*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}

	return;
}

bool SoundSecondClass::playFile(std::string name, bool loop)
{
	HRESULT result;
	IDirectSoundBuffer8* sound = sounds_.at(name);
	DWORD status;
	sound->GetStatus(&status);
	if(!(status && DSBSTATUS_PLAYING))
	{
		// Set position at the beginning of the sound buffer.
		result = sound->SetCurrentPosition(0);
		if(FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = sound->SetVolume(DSBVOLUME_MAX);
		if(FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		if(loop)
		{
			result = sound->Play(0, 0, DSBPLAY_LOOPING);
		}
		else
		{
			result = sound->Play(0, 0, 0);
		}
		if(FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool SoundSecondClass::isPlaying(std::string name)
{
	HRESULT result;
	IDirectSoundBuffer8* sound = sounds_.at(name);
	DWORD status;
	sound->GetStatus(&status);
	if((status && DSBSTATUS_PLAYING))
	{
		return true;
	}

	return false;
}

bool SoundSecondClass::playCelebrate()
{
	HRESULT result;
	
	if(!isPlaying("celebracion_1") && !isPlaying("celebracion_2"))
	{
		if(rand() % 2)
		{
			playFile("celebracion_1", false);
		}
		else
		{
			playFile("celebracion_2", false);
		}
	}

	return true;
}

bool SoundSecondClass::playHi()
{
	HRESULT result;
	
	if(!isPlaying("hola_1") && !isPlaying("hola_2"))
	{
		if(rand() % 2)
		{
			playFile("hola_1", false);
		}
		else
		{
			playFile("hola_2", false);
		}
	}

	return true;
}

bool SoundSecondClass::playPointing()
{
	HRESULT result;
	
	if(!isPlaying("pointing_1") && !isPlaying("pointing_2"))
	{
		if(rand() % 2)
		{
			playFile("pointing_1", false);
		}
		else
		{
			playFile("pointing_2", false);
		}
	}

	return true;
}

bool SoundSecondClass::playSurprise()
{
	HRESULT result;
	
	if(!isPlaying("sorprendido_1") && !isPlaying("sorprendido_2"))
	{
		if(rand() % 2)
		{
			playFile("sorprendido_1", false);
		}
		else
		{
			playFile("sorprendido_2", false);
		}
	}

	return true;
}
