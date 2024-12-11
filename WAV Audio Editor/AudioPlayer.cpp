#include "AudioPlayer.h"
// Конструктор с данными
AudioPlayer::AudioPlayer(HWND hWnd, const std::vector<char>& audioData, WAVEFORMATEXTENSIBLE waveFormat)
    : audioData(audioData), waveFormat(waveFormat), directSound(nullptr),
    primaryBuffer(nullptr), secondaryBuffer(nullptr),
    isPlaying(false), isPaused(false), isLooping(false),
    start_position(0), end_position(audioData.size()),
    playThreadHandle(nullptr), recordThreadHandle(nullptr), hWnd(hWnd),
    isRecording(false), directSoundCapture(nullptr), captureBuffer(nullptr), isRecordingPaused(false), isPaintIsDone(false) {
}

// Конструктор по умолчанию
AudioPlayer::AudioPlayer(HWND hWnd)
    : directSound(nullptr), primaryBuffer(nullptr), secondaryBuffer(nullptr),
    isPlaying(false), isPaused(false), isLooping(false),
    start_position(0), end_position(0),
    playThreadHandle(nullptr), recordThreadHandle(nullptr), hWnd(hWnd),
    isRecording(false), directSoundCapture(nullptr), captureBuffer(nullptr), isRecordingPaused(false), isPaintIsDone(false) {
}

// Деструктор
AudioPlayer::~AudioPlayer() {
    stop();
    releaseDirectSound();
}

bool AudioPlayer::get_isPaused() const {
    return isPaused.load();
}

bool AudioPlayer::get_isPaintIsDone() const
{
    return isPaintIsDone;
}

void AudioPlayer::set_isPaintIsDone(bool PaintIsDone)
{
    isPaintIsDone = PaintIsDone;
}

bool AudioPlayer::get_isPausedRecording() const
{
    return isRecordingPaused.load();
}

bool AudioPlayer::get_isRecording() const
{
    return isRecording.load();
}

bool AudioPlayer::get_isPlaying() const
{
    return isPlaying.load();
}

void AudioPlayer::set_audioData(const std::vector<char>& audioData)
{
    this->audioData = audioData;
}

std::vector<char>& AudioPlayer::get_audioData()
{
    return audioData;
}

void AudioPlayer::set_waveFormat(WAVEFORMATEXTENSIBLE waveFormat)
{
    this->waveFormat = waveFormat;
}

bool AudioPlayer::initializeDirectSound() {
    // Инициализация DirectSound
    if (FAILED(DirectSoundCreate8(NULL, &directSound, NULL))) {
        OutputDebugStringA("Ошибка инициализации DirectSound.\n");
        return false;
    }

    // Установка уровня кооперации
    if (FAILED(directSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY))) {
        OutputDebugStringA("Ошибка установки уровня кооперации DSSCL_PRIORITY.\n");
        return false;
    }

    // Создание primary буфера
    DSBUFFERDESC bufferDesc = { 0 };
    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

    if (FAILED(directSound->CreateSoundBuffer(&bufferDesc, &primaryBuffer, NULL))) {
        OutputDebugStringA("Ошибка создания primary буфера.\n");
        return false;
    }

    // Устанавливаем формат для primary буфера
    if (FAILED(primaryBuffer->SetFormat((WAVEFORMATEX*)&waveFormat))) {
        OutputDebugStringA("Ошибка установки формата primary буфера.\n");
        return false;
    }

    // Создание secondary буфера для аудио данных
    bufferDesc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2;
    bufferDesc.dwBufferBytes = end_position - start_position; // Размер буфера
    bufferDesc.lpwfxFormat = (WAVEFORMATEX*)&waveFormat; // Передаем формат с количеством каналов

    if (FAILED(directSound->CreateSoundBuffer(&bufferDesc, &secondaryBuffer, NULL))) {
        OutputDebugStringA("Ошибка создания secondary буфера.\n");
        return false;
    }

    return true;
}

bool AudioPlayer::initializeDirectSoundCapture() {
    // Создание интерфейса DirectSoundCapture
    if (FAILED(DirectSoundCaptureCreate8(NULL, &directSoundCapture, NULL))) {
        OutputDebugStringA("Ошибка инициализации DirectSoundCapture.\n");
        return false;
    }
    // Формат записи
    WAVEFORMATEX _waveFormat = {};
    if (waveFormat.SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
        _waveFormat.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    }
    else {
        if (waveFormat.SubFormat == KSDATAFORMAT_SUBTYPE_PCM) {
            _waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        }
        else {

        }
    }
    _waveFormat.nChannels = waveFormat.Format.nChannels;    // Используем текущие параметры
    _waveFormat.nSamplesPerSec = waveFormat.Format.nSamplesPerSec;
    _waveFormat.wBitsPerSample = waveFormat.Format.wBitsPerSample;
    _waveFormat.nBlockAlign = waveFormat.Format.nBlockAlign;
    _waveFormat.nAvgBytesPerSec = waveFormat.Format.nAvgBytesPerSec;
    _waveFormat.cbSize = 0;
    // Настройка буфера записи
    DSCBUFFERDESC bufferDesc = {};
    bufferDesc.dwSize = sizeof(DSCBUFFERDESC);
    bufferDesc.dwFlags = 0;
    bufferDesc.dwBufferBytes = waveFormat.Format.nAvgBytesPerSec * 2;
    bufferDesc.lpwfxFormat = (WAVEFORMATEX*)&_waveFormat;

    if (FAILED(directSoundCapture->CreateCaptureBuffer(&bufferDesc, &captureBuffer, NULL))) {
        DWORD error = GetLastError();
        char errorMsg[256];
        //sprintf(errorMsg, "Ошибка создания буфера захвата. Код ошибки: %lu", error);
        OutputDebugStringA(errorMsg);
        return false;
        OutputDebugStringA("Ошибка создания буфера захвата.\n");
        return false;
    }

    return true;
}



void AudioPlayer::releaseDirectSound() {
    if (secondaryBuffer) {
        secondaryBuffer->Release();
        secondaryBuffer = nullptr;
    }
    if (primaryBuffer) {
        primaryBuffer->Release();
        primaryBuffer = nullptr;
    }
    if (directSound) {
        directSound->Release();
        directSound = nullptr;
    }
}

bool AudioPlayer::fillBufferWithData(size_t start, size_t end) {
    if (!secondaryBuffer) return false;

    size_t bufferSize = end - start;
    void* bufferPtr1;
    DWORD bufferBytes1;
    void* bufferPtr2;
    DWORD bufferBytes2;

    if (FAILED(secondaryBuffer->Lock(0, bufferSize, &bufferPtr1, &bufferBytes1, &bufferPtr2, &bufferBytes2, 0))) {
        OutputDebugStringA("Ошибка блокировки буфера.\n");
        return false;
    }

    memcpy(bufferPtr1, audioData.data() + start, bufferBytes1);
    if (bufferPtr2 != nullptr) {
        memcpy(bufferPtr2, audioData.data() + start + bufferBytes1, bufferBytes2);
    }

    secondaryBuffer->Unlock(bufferPtr1, bufferBytes1, bufferPtr2, bufferBytes2);
    return true;
}

void AudioPlayer::play() {
    //if (isPlaying) {
        stop();
    //}
    if (start_position == end_position)
        return;
    if (!initializeDirectSound())
        return;
    isPlaying = true;
    isPaused = false;

    fillBufferWithData(start_position, end_position);
    playThreadHandle = CreateThread(NULL, 0, playbackLoop, this, 0, NULL);
}

void AudioPlayer::pause() {
    if (isPlaying && !isPaused) {
        isPaused = true;
        if (secondaryBuffer == NULL)
            return;
        secondaryBuffer->Stop();
    }
}

void AudioPlayer::resume() {
    if (isPlaying && isPaused) {
        isPaused = false;
        if (secondaryBuffer == NULL)
            return;
        secondaryBuffer->Play(0, 0, isLooping ? DSBPLAY_LOOPING : 0);
    }
}

void AudioPlayer::loop() {
    isLooping = !isLooping;
    if (isPlaying && !isPaused) {
        //secondaryBuffer->Play(0, 0, isLooping ? DSBPLAY_LOOPING : 0);
    }
}

void AudioPlayer::stop() {
    if (isPlaying) {
        isPlaying = false;
        if (playThreadHandle) {
            WaitForSingleObject(playThreadHandle, INFINITE);
            CloseHandle(playThreadHandle);
            playThreadHandle = nullptr;
        }
        if (secondaryBuffer == NULL)
            return ;
        secondaryBuffer->Stop();
        secondaryBuffer->SetCurrentPosition(0);
    }
    else {
        if (playThreadHandle) {
            WaitForSingleObject(playThreadHandle, INFINITE);
            CloseHandle(playThreadHandle);
            playThreadHandle = nullptr;
        }
    }
}

void AudioPlayer::startRecording() {
    if (isRecording.load()) return;

    if (!initializeDirectSoundCapture()) return;

    isRecording = true;
    InitializeCriticalSection(&recordedDataLock);
    recordedData.clear();

    recordThreadHandle = CreateThread(NULL, 0, recordingLoop, this, 0, NULL);
    captureBuffer->Start(DSCBSTART_LOOPING);
}

void AudioPlayer::pauseRecording() {
    if (isRecording && !isRecordingPaused) {
        captureBuffer->Stop();
        isRecordingPaused = true;
    }
}

void AudioPlayer::resumeRecording() {
    if (isRecording && isRecordingPaused) {
        captureBuffer->Start(DSCBSTART_LOOPING);
        isRecordingPaused = false;
    }
}

void AudioPlayer::stopRecording() {
    if (!isRecording.load()) return;

    isRecording = false;

    if (recordThreadHandle) {
        WaitForSingleObject(recordThreadHandle, INFINITE);
        CloseHandle(recordThreadHandle);
        recordThreadHandle = NULL;
    }

    captureBuffer->Stop();
    captureBuffer->Release();
    captureBuffer = nullptr;

    DeleteCriticalSection(&recordedDataLock);
}

std::vector<char> AudioPlayer::getRecordedData() {
    EnterCriticalSection(&recordedDataLock);
    std::vector<char> copy = recordedData;
    LeaveCriticalSection(&recordedDataLock);
    return copy;
}



void AudioPlayer::move_to_start() {
    set_start_position(0);
    set_end_position(audioData.size());
}

void AudioPlayer::move_to_end() {
    set_end_position(audioData.size());
    set_start_position(audioData.size());
}

void AudioPlayer::set_start_position(size_t start_position) {
    this->start_position = start_position < audioData.size() ? start_position : audioData.size();
}

void AudioPlayer::set_end_position(size_t end_position) {
    this->end_position = end_position < audioData.size() ? end_position : audioData.size();
}

DWORD WINAPI AudioPlayer::playbackLoop(LPVOID lpParam) {
    AudioPlayer* player = static_cast<AudioPlayer*>(lpParam);
    if (player->secondaryBuffer == NULL)
        return 1;
    player->secondaryBuffer->SetCurrentPosition(0);
    player->secondaryBuffer->Play(0, 0, player->isLooping ? DSBPLAY_LOOPING : 0);
    int max = 0;
    bool isEnd = false;
    while (player->isPlaying) {
        if (player->isPaused) {
            Sleep(100);
            continue;
        }
        DWORD playPos, writePos;
        player->secondaryBuffer->GetCurrentPosition(&playPos, &writePos);
        if (playPos >= max) {
            max = playPos;
            isEnd = false;
        }
        else
            isEnd = true;
        if (isEnd && !player->isLooping) {
            player->isPlaying = false;
            player->secondaryBuffer->Stop();
            player->secondaryBuffer->SetCurrentPosition(0);
            break;
        }
        Sleep(100);
    }

    return 0;
}

DWORD WINAPI AudioPlayer::recordingLoop(LPVOID lpParam) {
    AudioPlayer* player = static_cast<AudioPlayer*>(lpParam);
    const DWORD bufferSize = player->waveFormat.Format.nAvgBytesPerSec; // Размер буфера на 1 секунду
    DWORD lastReadPos = 0; // Начальная позиция чтения
    std::vector<char> tempBuffer(bufferSize);
    int count = 0;
    int time = 1500;
    while (player->isRecording.load()) {
        if (player->isRecordingPaused) {
            Sleep(100);
            continue;
        }
        DWORD capturePos, readPos;
        player->captureBuffer->GetCurrentPosition(&capturePos, &readPos);

        // Рассчитываем количество новых данных
        DWORD bytesToRead = (capturePos >= lastReadPos)
            ? (capturePos - lastReadPos) // Данные в пределах буфера
            : (bufferSize - lastReadPos + capturePos); // Данные с переходом через границу

        if (bytesToRead > 0) {
            // Считываем данные из буфера
            void* audioPtr1;
            DWORD audioBytes1;
            void* audioPtr2;
            DWORD audioBytes2;

            if (SUCCEEDED(player->captureBuffer->Lock(lastReadPos, bytesToRead, &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, 0))) {
                memcpy(tempBuffer.data(), audioPtr1, audioBytes1);
                if (audioPtr2) {
                    memcpy(tempBuffer.data() + audioBytes1, audioPtr2, audioBytes2);
                }
                player->captureBuffer->Unlock(audioPtr1, audioBytes1, audioPtr2, audioBytes2);

                EnterCriticalSection(&player->recordedDataLock);
                player->recordedData.insert(player->recordedData.end(), tempBuffer.begin(), tempBuffer.begin() + bytesToRead);
                LeaveCriticalSection(&player->recordedDataLock);
            }
        }

        lastReadPos = capturePos;
        Sleep(100);
        count++;
        if (count >= 10) {
            if (player->isPaintIsDone) {
                PostMessage(player->hWnd, WM_UPDATE_RECORDED_AUDIO, 0, 0);
                while (!player->MessageIsGotten) {}
                player->MessageIsGotten = false;
                player->recordedData.clear();
                count = 0;
            }
        }
    }

    return 0;
}



