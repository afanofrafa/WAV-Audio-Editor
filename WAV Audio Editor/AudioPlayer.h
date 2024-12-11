#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <vector>
#include <windows.h>
#include <dsound.h>
#include <mmsystem.h>
#include <atomic>
#include <mmreg.h>

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#define WM_UPDATE_RECORDED_AUDIO WM_USER + 1

class AudioPlayer {
public:
    AudioPlayer(HWND hWnd, const std::vector<char>& audioData, WAVEFORMATEXTENSIBLE waveFormat);
    AudioPlayer(HWND hWnd);
    ~AudioPlayer();

    bool get_isPaused() const;
    bool get_isPaintIsDone() const;
    void set_isPaintIsDone(bool PaintIsDone);
    bool get_isPausedRecording() const;
    bool get_isRecording() const;
    bool get_isPlaying() const;
    void set_audioData(const std::vector<char>& audioData);
    std::vector<char>& get_audioData();
    void set_waveFormat(WAVEFORMATEXTENSIBLE waveFormat);
    void set_start_position(size_t start_position);
    void set_end_position(size_t end_position);

    void play();
    void move_to_start();
    void move_to_end();
    void pause();
    void resume();
    void loop();
    void stop();
    void startRecording();
    void pauseRecording();
    void resumeRecording();
    void stopRecording();
    std::vector<char> getRecordedData();

    bool moveStartCalled = false;
    bool moveEndCalled = false;
    bool MessageIsGotten = false;
    bool isPaintIsDone = false;
private:
    bool initializeDirectSound();
    bool initializeDirectSoundCapture();
    void releaseDirectSound();
    bool fillBufferWithData(size_t start, size_t end);
    static DWORD WINAPI playbackLoop(LPVOID lpParam);
    static DWORD WINAPI recordingLoop(LPVOID lpParam);

    // DirectSound components
    LPDIRECTSOUND8 directSound;
    LPDIRECTSOUNDBUFFER primaryBuffer;
    LPDIRECTSOUNDBUFFER secondaryBuffer;

    WAVEFORMATEXTENSIBLE waveFormat;
    std::vector<char> audioData;

    // Playback control
    std::atomic<bool> isPlaying;
    std::atomic<bool> isPaused;
    std::atomic<bool> isLooping;
    std::atomic<bool> isRecording;  
    std::atomic<bool> isRecordingPaused;

    std::vector<char> recordedData;        
    LPDIRECTSOUNDCAPTURE8 directSoundCapture;   
    LPDIRECTSOUNDCAPTUREBUFFER captureBuffer;
    HANDLE recordThreadHandle;                  
    CRITICAL_SECTION recordedDataLock;          

    size_t start_position;
    size_t end_position;

    HWND hWnd;

    HANDLE playThreadHandle;
};
#endif // AUDIO_PLAYER_H