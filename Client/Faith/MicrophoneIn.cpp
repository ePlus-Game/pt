#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "MicroPhoneIn.h"


MicrophoneIn::MicrophoneIn()
{
	hMicrophoneWaveIn = 0;
	pMirophoneWaveHDR = 0;
	pMirocphoneBuffer = 0;
	fMicrophonePower = 0.0f;
	hCurrentWindow = 0;
	deviceID = 0;
	currentLength = 0;
	pMicroPhonemode = 8;
	pSaveBuffer = 0;
}

MicrophoneIn::~MicrophoneIn()
{
	//////////没有释放任何东西，用这个类的时候释放请用Destroy///////////////
	/////////另外响应消息MM_WIM_CLOSE///////////////////////
}
BOOL MicrophoneIn::MicroPhoneProcessMM_WIM_OPEN(WPARAM wParam,LPARAM lParam)
{
	waveInAddBuffer(hMicrophoneWaveIn,pMirophoneWaveHDR,sizeof(WAVEHDR));
	waveInStart(hMicrophoneWaveIn);
	dwRecordLength = 0;
	pSaveBuffer = new BYTE[_MICROPHONE_SAVE_BUFFER_MAX_SIZE];
	
	fwTime = (float)timeGetTime();
	return TRUE;
}
BOOL MicrophoneIn::MicroPhoneDestroy()
{
	waveInClose(hMicrophoneWaveIn);
	return TRUE;
}
BOOL MicrophoneIn::MicroPhoneReset()
{
	waveInReset(hMicrophoneWaveIn);
	return TRUE;
}
BOOL MicrophoneIn::MicroPhoneProcessMM_WIM_DATA(WPARAM wParam,LPARAM lParam)
{
	LPWAVEHDR pWaveHdr = (LPWAVEHDR)lParam;
	waveInUnprepareHeader(hMicrophoneWaveIn,pWaveHdr,sizeof(WAVEHDR));
	fMicrophonePower = 0.0f;
	if(pMicroPhonemode==_MICROPHONE_GET_VOLUME_IN_BUFFER)
	{
		if(_MICROPHONE_WAVE_BITS_PER_SMAPLE == 8)
		{
			UCHAR* pBuffer = (UCHAR*)pWaveHdr->lpData;
			for(UINT i = 0; i < pWaveHdr->dwBytesRecorded;i++)
			{
				float power = (((float)(pBuffer[i]-128))/128.0f);
				if(power<0.0f)
					power= -power;
				fMicrophonePower+=power;
			}
			fMicrophonePower /= (float)pWaveHdr->dwBytesRecorded;
		}
		else
		if(_MICROPHONE_WAVE_BITS_PER_SMAPLE == 16)
		{
			SHORT* pBuffer = (SHORT*)pWaveHdr->lpData;
			int size = pWaveHdr->dwBytesRecorded/2;
			for(int i = 0; i < size;i++)
			{
				float power = ((float)pBuffer[i])/(((float)66535)/2.0f);
				if(power < 0.0f)
					power = -power;
				fMicrophonePower+=power;
			}
			fMicrophonePower/=(float)size;
		}
	}
	else
	{
		float time = (float) timeGetTime();
		float dTime = (time - fwTime)/1000.0f;
		if(dTime>=(float)_MICROPHONE_VOLUME_IN_TIME)
		{
			if(_MICROPHONE_WAVE_BITS_PER_SMAPLE == 8)
			{
				UCHAR* pBuffer = (UCHAR*)pSaveBuffer;
				for(UINT i = 0; i < dwRecordLength;i++)
				{
					float power = (((float)(pBuffer[i]-128))/128.0f);
					if(power<0.0f)
						power= -power;
					fMicrophonePower+=power;
				}
				fMicrophonePower /= (float)dwRecordLength;
			}
		    else
		    if(_MICROPHONE_WAVE_BITS_PER_SMAPLE == 16)
			{
				SHORT* pBuffer = (SHORT*)pSaveBuffer;
				int size = dwRecordLength/2;
				for(int i = 0; i < size;i++)
				{
					float power = ((float)pBuffer[i])/(((float)66535)/2.0f);
					if(power < 0.0f)
						power = -power;
					fMicrophonePower+=power;
				}
				fMicrophonePower/=(float)size;
			}
			if(dwRecordLength>_MICROPHONE_SAVE_BUFFER_MAX_SIZE)
			{
				delete [] pSaveBuffer;
				pSaveBuffer = 0;
				pSaveBuffer = new BYTE[_MICROPHONE_SAVE_BUFFER_MAX_SIZE];
			}
			
			dwRecordLength  = 0;
			fwTime = (float)timeGetTime();
		}
		else
		{
			if(dwRecordLength+pWaveHdr->dwBytesRecorded<=_MICROPHONE_SAVE_BUFFER_MAX_SIZE)
			{
				CopyMemory (pSaveBuffer + dwRecordLength, pWaveHdr->lpData,
                         pWaveHdr->dwBytesRecorded);
	     		dwRecordLength += pWaveHdr->dwBytesRecorded;
			}
			else
			{
				PBYTE pNewBuffer = new BYTE[dwRecordLength+pWaveHdr->dwBytesRecorded];
				memcpy(pNewBuffer,pSaveBuffer,dwRecordLength);
				memcpy(pNewBuffer+dwRecordLength,pWaveHdr->lpData,pWaveHdr->dwBytesRecorded);
				delete [] pSaveBuffer;
				pSaveBuffer = pNewBuffer;
				dwRecordLength += pWaveHdr->dwBytesRecorded;
			}
			
		}

	}
	waveInPrepareHeader(hMicrophoneWaveIn,pWaveHdr,sizeof(WAVEHDR));
	waveInAddBuffer(hMicrophoneWaveIn,pWaveHdr,sizeof(WAVEHDR));
	return TRUE;
}
BOOL MicrophoneIn::MicroPhoneProcessMM_WIM_CLOSE(WPARAM wParam ,LPARAM lParam)
{
	waveInUnprepareHeader(hMicrophoneWaveIn,pMirophoneWaveHDR,sizeof(WAVEHDR));
	delete [] pMirocphoneBuffer;
	pMirocphoneBuffer = 0;
	pMirophoneWaveHDR->lpData = 0;
	delete pMirophoneWaveHDR;
	if(pSaveBuffer)
	{
		delete [] pSaveBuffer;
		pSaveBuffer = 0;
	}
	pMirophoneWaveHDR = 0;
	return TRUE;
}
void MicrophoneIn::MicrophonePrepare()
{
	pMirophoneWaveHDR = new WAVEHDR;
	pMirocphoneBuffer = new BYTE[_MICROPHONE_DEFAULT_BUFFER_SIZE*(_MICROPHONE_WAVE_BITS_PER_SMAPLE>>1)];
	MicrophoneInSetWaveHDR(pMirophoneWaveHDR,pMirocphoneBuffer,_MICROPHONE_DEFAULT_BUFFER_SIZE*(_MICROPHONE_WAVE_BITS_PER_SMAPLE>>1));
	waveInPrepareHeader(hMicrophoneWaveIn,pMirophoneWaveHDR,sizeof(WAVEHDR));
}
BOOL MicrophoneIn::MicrophoneStart(HWND hwnd,int flags,int mode)
{
	pMicroPhonemode = mode;
	hCurrentWindow = hwnd;
	if(waveInGetNumDevs()==0)
		return FALSE;
	deviceID = 0;
	waveInGetDevCaps(deviceID,&waveCaps,sizeof(WAVEINCAPS));
	MicrophoneInSetWaveFormat(&waveFormat);
	if(flags == _MICROPHONE_CREATE_CALLBACK_WINDOW)
	{
		if(waveInOpen(&hMicrophoneWaveIn,deviceID,&waveFormat,(DWORD)hwnd,0,CALLBACK_WINDOW))
		{
			return FALSE;
		}
		MicrophonePrepare();
	}
	else
	{
		if(waveInOpen(&hMicrophoneWaveIn,deviceID,&waveFormat,
			(DWORD)MircophoneProcessProc,(DWORD)this,CALLBACK_FUNCTION))
		{
			return FALSE;
		}
		MicrophonePrepare();
		MicroPhoneProcessMM_WIM_OPEN(0,0);
	}
	return TRUE;
}

void CALLBACK MircophoneProcessProc(HWAVEIN hWaveIn,UINT msg,DWORD userData,DWORD param1,DWORD param2)
{
	switch(msg)
	{
	case WIM_OPEN:
		{
			///////当为回调函数时候WIM_IPEN不需要处理任何东西////////////
		}
		break;
	case WIM_CLOSE:		
		((LPMICROPHONEIN)userData)->MicroPhoneProcessMM_WIM_CLOSE(0,0);
		break;
	case WIM_DATA:
		((LPMICROPHONEIN)userData)->MicroPhoneProcessMM_WIM_DATA(0,param1);		
		break;
	}
}