/*-------------烈火*张鹏程-----------------*/





#ifndef _MICROPHONE_IN_H
#define _MICROPHONE_IN_H
/*把声音的处理消息发送到指定窗口函数*/
#define _MICROPHONE_CREATE_CALLBACK_WINDOW  0
//把声音的处理消息发送到指定函数*/
#define _MICROPHONE_CREATE_CALLBACK_FUNCTION 1

//录音的时候填满缓冲区的大小*/
#define _MICROPHONE_DEFAULT_BUFFER_SIZE        300
//采样样本位数//
#define _MICROPHONE_WAVE_BITS_PER_SMAPLE        8
//采样频率//
#define _MICROPHONE_WAVE_SMAPLE                22050

///时间模式中的获取声音值的间隔时间////////////
#define _MICROPHONE_VOLUME_IN_TIME             0.1
/////获取声音值的模式////////////////
/////buffer模式/////////////////////////

#define _MICROPHONE_GET_VOLUME_IN_BUFFER      0

////时间模式///////////////////////////
#define _MICROPHONE_GET_VOLUME_IN_TIME        1

////for 时间模式，如果是时间模式，把_MICROPHONE_DEFAULT_BUFFER_SIZE 设置为1比较好
#define _MICROPHONE_SAVE_BUFFER_MAX_SIZE      20000



///////////////////////////////
//设置wave流的头信息//
inline void MicrophoneInSetWaveHDR(LPWAVEHDR pWaveHDR,LPVOID pWaveBuffer,int size)
{

	pWaveHDR->lpData          = (char*)pWaveBuffer ;
    pWaveHDR->dwBufferLength  = size ;
    pWaveHDR->dwBytesRecorded = 0 ;
    pWaveHDR->dwUser          = 0 ;
    pWaveHDR->dwFlags         = 0 ;
    pWaveHDR->dwLoops         = 1 ;
    pWaveHDR->lpNext          = NULL ;
    pWaveHDR->reserved        = 0 ;
	
}
//设置wave格式//
inline void MicrophoneInSetWaveFormat(LPWAVEFORMATEX pWaveFormat)
{
	pWaveFormat->cbSize = 0;
	pWaveFormat->nSamplesPerSec = _MICROPHONE_WAVE_SMAPLE;
	pWaveFormat->wBitsPerSample = _MICROPHONE_WAVE_BITS_PER_SMAPLE;
	pWaveFormat->nChannels = 1;
	pWaveFormat->nBlockAlign = (pWaveFormat->wBitsPerSample>>3)*pWaveFormat->nChannels;
	pWaveFormat->nAvgBytesPerSec = pWaveFormat->nBlockAlign* pWaveFormat->nSamplesPerSec;
	pWaveFormat->wFormatTag = WAVE_FORMAT_PCM;

}
///////////////////////////////
//获取音量的麦克风类//
void CALLBACK MircophoneProcessProc(HWAVEIN hWaveIn,UINT msg,DWORD userData,DWORD param1,DWORD param2);
typedef class MicrophoneIn
{
public:
	MicrophoneIn();
	~MicrophoneIn();
	//获取硬件接口,并且发送MM_WIM_OPEN消息,flgas为_MICROPHONE_CREATE_CALLBACK_WINDOW,表示处理声音消息是窗口
	//这样需要在窗口函数中自己添加消息//
	BOOL MicrophoneStart(HWND hwnd,int flags,int mode = _MICROPHONE_GET_VOLUME_IN_BUFFER);
	//处理MM_WIM_OPEN消息//
	BOOL MicroPhoneProcessMM_WIM_OPEN(WPARAM wParam,LPARAM lParam);
	//处理MM_WIM_DATA消息//并且计算声音强度
	BOOL MicroPhoneProcessMM_WIM_DATA(WPARAM wParam,LPARAM lParam);
	//处理MM_WIM_CLOSE//
	BOOL MicroPhoneProcessMM_WIM_CLOSE(WPARAM wParam ,LPARAM lParam);
	//重置硬件,当缓冲区未填满时候,要停止录音可调这个函数//
	BOOL MicroPhoneReset();
	//................
	BOOL MicroPhoneDestroy();
	void MicrophonePrepare();

public:
	HWAVEIN   MicroPhoneGetDevice() const{return hMicrophoneWaveIn;}
	float     MicroPhoneGetPower() const {return fMicrophonePower;}
	HWND      MicroPhoneGetCurrentWindow() const { return hCurrentWindow;}
	int       MicroPhoneGetDeviceID() const { return deviceID;} 
protected:
	int     getVolumeMode;
	//硬件
	HWAVEIN  hMicrophoneWaveIn;
	//头
	PWAVEHDR pMirophoneWaveHDR;
	//buffer
	LPVOID   pMirocphoneBuffer;
	//强度,是一个0-1的浮点数
	float    fMicrophonePower;
	HWND     hCurrentWindow;
	int      deviceID;
	int      currentLength;
	int      pMicroPhonemode;
	WAVEFORMATEX  waveFormat;
	WAVEINCAPS    waveCaps;
	float         fwTime;
	LPBYTE        pSaveBuffer;
	UINT          dwRecordLength;
}MICROPHONEIN,*LPMICROPHONEIN;

#endif