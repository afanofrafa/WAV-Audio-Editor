//L"C:\\Users\\Archie\\source\\repos\\Coursework_sem5_sounnds\\Coursework_sem5_sounnds"
#include <windows.h>
//#include <commdlg.h>  // Для диалога открытия файла
#include <commctrl.h>
#include <vector>
#include <mmreg.h>
#include <fstream>    
#include <sstream>
#include <cstdint>    // Для определения типов данных
#include <string>
#include <iomanip>
//#include <mmsystem.h> // Для функций waveOut
//#include <sstream>
//#include <filesystem>
#include "Resource.h"
#include "Coursework_sem5_sounnds.h"
#include "WaveformRenderer.h"
#include "AudioEncodingConverter.h"
#include "Buffer.h"
#include "ButtonsDrawings.h"
#include "Selection.h"
#include "AudioPlayer.h"
#include "AudioEditor.h"
#include "Templates.h"
#include "Logger.h"
#define MAX_CHANNELS 32
#define START_X_POS 10
// Все каналы, которые можно активировать (Список каналов для маски)
const std::vector<DWORD> speakerChannels = {
    SPEAKER_FRONT_LEFT,           // 0x1
    SPEAKER_FRONT_RIGHT,          // 0x2
    SPEAKER_FRONT_CENTER,         // 0x4
    SPEAKER_LOW_FREQUENCY,        // 0x8
    SPEAKER_BACK_LEFT,            // 0x10
    SPEAKER_BACK_RIGHT,           // 0x20
    SPEAKER_FRONT_LEFT_OF_CENTER, // 0x40
    SPEAKER_FRONT_RIGHT_OF_CENTER,// 0x80
    SPEAKER_BACK_CENTER,          // 0x100
    SPEAKER_SIDE_LEFT,            // 0x200
    SPEAKER_SIDE_RIGHT,           // 0x400
    SPEAKER_TOP_CENTER,           // 0x800
    SPEAKER_TOP_FRONT_LEFT,       // 0x1000
    SPEAKER_TOP_FRONT_CENTER,     // 0x2000
    SPEAKER_TOP_FRONT_RIGHT,      // 0x4000
    SPEAKER_TOP_BACK_LEFT,        // 0x8000
    SPEAKER_TOP_BACK_CENTER,      // 0x10000
    SPEAKER_TOP_BACK_RIGHT,       // 0x20000
    SPEAKER_TOP_BACK_RIGHT * 2,
    SPEAKER_TOP_BACK_RIGHT * 4,
    SPEAKER_TOP_BACK_RIGHT * 8,
    SPEAKER_TOP_BACK_RIGHT * 16,
    SPEAKER_TOP_BACK_RIGHT * 32,
    SPEAKER_TOP_BACK_RIGHT * 64,
    SPEAKER_TOP_BACK_RIGHT * 128,
    SPEAKER_TOP_BACK_RIGHT * 256,
    SPEAKER_TOP_BACK_RIGHT * 512,
    SPEAKER_TOP_BACK_RIGHT * 1024,
    SPEAKER_TOP_BACK_RIGHT * 2048,
    SPEAKER_TOP_BACK_RIGHT * 4096,
    SPEAKER_TOP_BACK_RIGHT * 4096 * 2,
    (DWORD)(SPEAKER_TOP_BACK_RIGHT * 4096 * 4),
};

// Подключаем необходимую библиотеку
//#pragma comment(lib, "winmm.lib")
//#pragma comment(lib, "gdiplus.lib")
struct WAVHeader {
    char chunkId[4];       // "RIFF"
    uint32_t chunkSize;     // Размер всего файла - 8 байт
    char format[4];         // "WAVE"

    char subChunk1Id[4];    // "fmt "
    uint32_t subChunk1Size; // Размер подблока (16 для PCM)
    uint16_t audioFormat;   // Формат аудио (1 = PCM)
    uint16_t numChannels;   // Количество каналов
    uint32_t sampleRate;    // Частота дискретизации
    uint32_t byteRate;      // Байтовая скорость (sampleRate * numChannels * bitsPerSample / 8)
    uint16_t blockAlign;    // Количество байт на сэмпл (numChannels * bitsPerSample / 8)
    uint16_t bitsPerSample; // Количество бит на сэмпл

    char subChunk2Id[4];    // "data"
    uint32_t subChunk2Size; // Размер данных
};
#ifndef _TYPES
#define _TYPES
enum types {
    _UINT8,
    _INT16,
    _INT24,
    _INT32,
    _FLOAT
};
#endif
enum ButtonsNames {
    START,
    PAUSE,
    FINISH,
    MOVE_TO_START,
    MOVE_TO_END,
    LOOP,
    RECORD
};
std::vector<std::vector<POINT>> points;
bool isRepaint = true;
int cnt = 0;
int currFirstChannel = -1;
//int currSecondChannel = -1;
int _typeSize = 0;
uint8_t currentType = -1;
//std::vector<Gdiplus::Bitmap*> v_images;
HWND hButtonPause_Continue;
HWND hButtonPlay;
HWND hButtonRecord;
HWND hButtonStop;
HWND hButtonApplySettings;
HWND hButtonCyclePlaying;
HWND hButtonUnloadData;
HWND hButtonMoveToEnd;
HWND hButtonMoveToStart;
HWND hButtonMoveUp;
HWND hButtonMoveDown;
HWND hButtonLoadFile;
HWND hButtonSaveFile;
HWND hComboBoxEncodingChoice;
HWND hComboBoxDiscretisationChoice;
HWND hLabelSettings;
HWND hLabelEncoding;
HWND hLabelSpeed;
HWND hLabelDiscretisation;
HWND hEditSpinBox; 
HWND hEditSpinBoxRecord;
HWND hEditFileName; 
HWND hSpinBoxUpDown;
HWND hSpinBoxUpDownRecord;
HWND hLabelFirtChannel;
HWND hLabelSecondChannel;
const double MIN_VALUE = 0.01;
const double MAX_VALUE = 100.0;
const double STEP = 0.01;
bool isWider = false;
bool updating = false;
std::vector<Buffer*> v_buffers;
//vector<POINT> points;
bool wm_size = false;
bool isMove = false;
bool firstRun = true;
bool LoopCondition = false;
bool RecordCondition = false;
bool PauseCondition = false;
bool isInitialization = true;
//bool DataWasChanged = false;
Selection* selection = nullptr;
Buffer* screen_buffer = nullptr;
AudioPlayer* audio_player = nullptr;
AudioEncodingConverter* audioEncodingConverter = nullptr;

WaveformRendererVariant w;
//AudioEditorVariant audio_editor;
//AudioDataVariant audioData;
std::vector<AudioEditorVariant> audio_editor;
std::vector<AudioEditor<char>*> audio_editorCh;
std::vector<AudioEditor<char>*> audio_editorChOrig;

std::vector<AudioDataVariant> audioChannels;
std::vector<std::vector<char>*> audioPlayerChannels; 
std::vector<std::vector<char>*> audioPlayerChannelsOriginal;

WCHAR selectedFilePath[MAX_PATH] = L"";
WAVEFORMATEXTENSIBLE waveFormat;
WAVEHDR waveHdr;
HWAVEOUT hWaveOut;
float scale = 5.0f;
int offset = 0;

std::vector<char> resampleBySpeed(
    float speedFactor,             
    const std::vector<char>& inputSamples, 
    uint8_t type              
) 
{
    AudioDataVariant audioData;
    setAudioDataByType(audioData, inputSamples, type);   
    std::vector<char> v = resampleAudioData(audioData, speedFactor, type);
    return v;
}

// Утилита для форматирования числа с точностью 2 знака после запятой
std::wstring formatValue(double value) {
    std::wostringstream stream;
    stream << std::fixed << std::setprecision(2) << value;
    return stream.str();
}
// Обновление значения Edit Control
void updateEditValue(HWND hEdit, double value) {
    if (value < MIN_VALUE) {
        value = MIN_VALUE;
        SetWindowText(hEdit, formatValue(value).c_str());
        return;
    }
    if (value > MAX_VALUE) {
        value = MAX_VALUE;
        SetWindowText(hEdit, formatValue(value).c_str());
        return;
    }
    if (isWider) {
        SetWindowText(hEdit, formatValue(value).c_str());
        return;
    }
    return;
}

// Получение значения из Edit Control
double getEditValue(HWND hEdit) {
    wchar_t buffer[256];
    for (int i = 0; i < 256; i++)
        buffer[i] = '\0';
    GetWindowText(hEdit, buffer, 256);
    if (buffer[5] != '\0') {
        buffer[5] = '\0';
        isWider = true;
    }
    else {
        if (buffer[0] == '0' && buffer[4] != '\0') {
            buffer[4] = '\0';
            isWider = true;
        }
        else {
            isWider = false;
        }
    }
    try {
        return std::stod(buffer);
    }
    catch (...) {
        return MIN_VALUE;
    }
}
bool saveWavFile(std::wstring& fileName, const WAVEFORMATEXTENSIBLE& waveFormat, const std::vector<char>& data) {
    // Открываем файл для записи в бинарном режиме
    std::ofstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        MessageBox(NULL, L"Не удалось открыть файл для записи", L"Ошибка", MB_OK | MB_ICONERROR);
        return false;
    }

    // Общие размеры
    uint32_t dataChunkSize = static_cast<uint32_t>(data.size());
    uint32_t fmtChunkSize = 16;
    uint32_t riffChunkSize = 20 + fmtChunkSize + dataChunkSize; // Размер данных RIFF

    // 1. RIFF-заголовок
    file.write("RIFF", 4);                              // "RIFF"
    file.write(reinterpret_cast<const char*>(&riffChunkSize), sizeof(riffChunkSize)); // Общий размер RIFF
    file.write("WAVE", 4);                              // "WAVE"

    // 2. fmt-заголовок
    file.write("fmt ", 4);                              // "fmt "
    file.write(reinterpret_cast<const char*>(&fmtChunkSize), sizeof(fmtChunkSize)); // Размер fmt-блока
    uint16_t audioFormat;
    if (waveFormat.SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT) {
        audioFormat = 3;
        file.write(reinterpret_cast<const char*>(&audioFormat), 2);
    }
    else {
        if (waveFormat.SubFormat == KSDATAFORMAT_SUBTYPE_PCM) {
            audioFormat = 1;
            file.write(reinterpret_cast<const char*>(&audioFormat), 2);
        }
    }
    file.write(reinterpret_cast<const char*>(&waveFormat.Format.nChannels), 2); 
    file.write(reinterpret_cast<const char*>(&waveFormat.Format.nSamplesPerSec), 4); 
    file.write(reinterpret_cast<const char*>(&waveFormat.Format.nAvgBytesPerSec), 4); 
    file.write(reinterpret_cast<const char*>(&waveFormat.Format.nBlockAlign), 2); 
    file.write(reinterpret_cast<const char*>(&waveFormat.Format.wBitsPerSample), 2); 

    // 3. data-заголовок
    file.write("data", 4);                              // "data"
    file.write(reinterpret_cast<const char*>(&dataChunkSize), sizeof(dataChunkSize)); // Размер data-блока
    file.write(data.data(), dataChunkSize);             // Аудиоданные

    // Закрываем файл
    file.close();
    std::wstring additional = L"Файл успешно сохранён с именем ";
    fileName = additional + fileName;
    MessageBox(NULL, fileName.c_str(), L"Информация", MB_OK | MB_ICONINFORMATION);

    return true;
}
bool _loadFile(const wchar_t* filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        MessageBox(NULL, L"Не удалось открыть файл", L"Ошибка", MB_OK | MB_ICONERROR);
        return false;
    }

    // Чтение заголовка WAV файла
    char chunkId[4];
    uint32_t chunkSize;
    char format[4];

    file.read(chunkId, 4);            // "RIFF"
    file.read(reinterpret_cast<char*>(&chunkSize), 4);  // Размер файла
    file.read(format, 4);             // "WAVE"

    // Проверка, что это WAV файл
    if (std::string(chunkId, 4) != "RIFF" || std::string(format, 4) != "WAVE") {
        MessageBox(NULL, L"Неправильный формат файла", L"Ошибка", MB_OK | MB_ICONERROR);
        return false;
    }

    // Чтение секции "fmt "
    char subChunk1Id[4];
    uint32_t subChunk1Size;
    uint16_t audioFormat, numChannels;
    uint32_t sampleRate, byteRate;
    uint16_t blockAlign, bitsPerSample;

    file.read(subChunk1Id, 4);        // "fmt "
    file.read(reinterpret_cast<char*>(&subChunk1Size), 4);
    file.read(reinterpret_cast<char*>(&audioFormat), 2);
    file.read(reinterpret_cast<char*>(&numChannels), 2);
    file.read(reinterpret_cast<char*>(&sampleRate), 4);
    file.read(reinterpret_cast<char*>(&byteRate), 4);
    file.read(reinterpret_cast<char*>(&blockAlign), 2);
    file.read(reinterpret_cast<char*>(&bitsPerSample), 2);
    if (numChannels < 1 || numChannels > 32)
        return false;
    wstring textEnc;
    if (audioFormat == 1) {  // PCM
        if (bitsPerSample == 8) {
            currentType = _UINT8;
            _typeSize = 1;
            textEnc = L"unsigned 8-bit";
        }
        else if (bitsPerSample == 16) {
            currentType = _INT16;
            _typeSize = 2;
            textEnc = L"signed 16-bit";
        }
        else if (bitsPerSample == 24) {
            currentType = _INT24;
            _typeSize = 3;
            textEnc = L"signed 24-bit";
        }
        else if (bitsPerSample == 32) {
            currentType = _INT32;
            _typeSize = 4;
            textEnc = L"signed 32-bit";
        }
        else {
            return false;
        }
    }
    else if (audioFormat == 3) {  // IEEE Float
        if (bitsPerSample == 32) {
            currentType = _FLOAT;
            _typeSize = 4;
            textEnc = L"IEEE float";
        }
        else {
            return false;
        }
    }
    // Проверка, что формат PCM
    /*if (std::string(subChunk1Id, 4) != "fmt " || audioFormat != 1) {
        MessageBox(NULL, L"Поддерживаются только PCM WAV файлы", L"Ошибка", MB_OK | MB_ICONERROR);
        return false;
    }*/

    // Пропуск дополнительных секций до "data"
    char subChunk2Id[4];
    uint32_t subChunk2Size;

    do {
        file.read(subChunk2Id, 4);
        file.read(reinterpret_cast<char*>(&subChunk2Size), 4);
        if (std::string(subChunk2Id, 4) != "data") {
            file.seekg(subChunk2Size, std::ios::cur);
        }
    } while (std::string(subChunk2Id, 4) != "data");

    // Чтение аудиоданных
    vector<char> data(subChunk2Size);
    //data.reserve(subChunk2Size);
    //data.resize(subChunk2Size);
    file.read(data.data(), subChunk2Size);
    file.close();
    int size = data.size() / numChannels;
    //if (audioData) {
    //    delete audioData;
    //    audioData = NULL;
    //}
    currFirstChannel = 0;
    audio_editorClear(audio_editor);
    audio_editor.reserve(numChannels);
    audio_editor.resize(numChannels);

    for (auto ptr : audio_editorCh) {
        if (ptr)
            delete ptr;
    }
    audio_editorCh.clear();
    audio_editorCh.reserve(numChannels);
    audio_editorCh.resize(numChannels);

    for (auto ptr : audio_editorChOrig) {
        if (ptr)
            delete ptr;
    }
    audio_editorChOrig.clear();
    audio_editorChOrig.reserve(numChannels);
    audio_editorChOrig.resize(numChannels);

    points.clear();
    points.reserve(numChannels);
    points.resize(numChannels);

    audioChannelsClear(audioChannels);
    audioChannels.reserve(numChannels);
    audioChannels.resize(numChannels);

    for (auto ptr : audioPlayerChannels) {
        if (ptr)
            delete ptr;
    }
    audioPlayerChannels.clear();
    audioPlayerChannels.reserve(numChannels);
    audioPlayerChannels.resize(numChannels);

    for (auto ptr : audioPlayerChannelsOriginal) {
        if (ptr)
            delete ptr;
    }
    audioPlayerChannelsOriginal.clear();
    audioPlayerChannelsOriginal.reserve(numChannels);
    audioPlayerChannelsOriginal.resize(numChannels);
    for (int i = 0; i < audioPlayerChannels.size(); i++) {
        audioPlayerChannels.at(i) = new std::vector<char>();
        audioPlayerChannels.at(i)->reserve(size);
        for (int j = 0; j < data.size(); j += _typeSize * numChannels) {
            for (int k = 0; k < _typeSize; k++) {
                audioPlayerChannels.at(i)->push_back(data.at(j + k + i * _typeSize));
            }
        }
        audioPlayerChannelsOriginal.at(i) = new vector<char>(*audioPlayerChannels.at(i));
        setAudioDataByType(audioChannels.at(i), *audioPlayerChannels.at(i), currentType);
    }
    audioEncodingConverter->set_basicData(audioPlayerChannelsOriginal);
    audioEncodingConverter->set_currentType(currentType);
    audioEncodingConverter->set_origType(currentType);
    audioEncodingConverter->set_typeSize(_typeSize);
    audioEncodingConverter->set_origTypeSize(_typeSize);
    //setAudioDataByType(audioData, data, currentType);

    waveFormat.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE; // Используем расширенный формат
    waveFormat.Format.nChannels = numChannels;             // Устанавливаем количество каналов
    waveFormat.Format.nSamplesPerSec = sampleRate;         // Частота дискретизации
    waveFormat.Format.wBitsPerSample = bitsPerSample;      // Количество бит на сэмпл
    waveFormat.Format.nBlockAlign = blockAlign;            // Размер блока (байт на сэмпл)
    waveFormat.Format.nAvgBytesPerSec = byteRate;          // Среднее количество байт в секунду
    waveFormat.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX); // Для WAVEFORMATEXTENSIBLE
    if (currentType == _FLOAT)
        waveFormat.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
    else
        waveFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
    waveFormat.Samples.wReserved = 0;
    waveFormat.Samples.wSamplesPerBlock = 0;
    waveFormat.Samples.wValidBitsPerSample = 0;
    waveFormat.dwChannelMask = 0;
    for (int i = 0; i < numChannels && i < speakerChannels.size(); i++) {
        waveFormat.dwChannelMask |= speakerChannels.at(i);
    }
    wstring textD = std::to_wstring(sampleRate);
    int count = SendMessage(hComboBoxDiscretisationChoice, CB_GETCOUNT, 0, 0);
    wchar_t buffer[256];
    for (int i = 0; i < count; ++i) {
        SendMessage(hComboBoxDiscretisationChoice, CB_GETLBTEXT, i, reinterpret_cast<LPARAM>(buffer)); 
        if (textD == buffer) {
            SendMessage(hComboBoxDiscretisationChoice, CB_SETCURSEL, i, 0);
            break;
        }
    }
    count = SendMessage(hComboBoxEncodingChoice, CB_GETCOUNT, 0, 0);
    for (int i = 0; i < count; ++i) {
        SendMessage(hComboBoxEncodingChoice, CB_GETLBTEXT, i, reinterpret_cast<LPARAM>(buffer));
        if (textEnc == buffer) {
            SendMessage(hComboBoxEncodingChoice, CB_SETCURSEL, i, 0);
            break;
        }
    }
    audio_player->set_audioData(data);
    audio_player->set_waveFormat(waveFormat);
    audio_player->set_start_position(0);
    audio_player->set_end_position(data.size() - 1);
    return true;
}
bool tryParseFloat(const std::wstring& s_speed, float& speed) {
    try {
        speed = std::stof(s_speed);
        return true; // Успех
    }
    catch (const std::invalid_argument&) {
        //std::wcerr << L"Ошибка: строка не содержит числа." << std::endl;
    }
    catch (const std::out_of_range&) {
        //std::wcerr << L"Ошибка: значение выходит за пределы диапазона float." << std::endl;
    }
    return false; // Ошибка
}

std::wstring GetFileNameFromPath(const std::wstring& filePath) {
    size_t found = filePath.find_last_of(L"\\");  // Находим последний разделитель (слэш или обратный слэш)
    if (found != std::wstring::npos) {
        return filePath.substr(found + 1);  // Возвращаем подстроку, начиная с символа после последнего слэша
    }
    return filePath;  // Если разделитель не найден, возвращаем путь целиком (в случае без пути)
}
// Функция для отображения диалогового окна выбора файла
void openFileDialog(HWND hwnd) {
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = selectedFilePath;
    ofn.lpstrFile[0] = '\0';  // Начальное значение
    ofn.nMaxFile = sizeof(selectedFilePath);
    ofn.lpstrFilter = L"Wave Files\0*.wav\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    //ofn.lpstrInitialDir = L"C:\\Users\\Archie\\source\\repos\\Coursework_sem5_sounnds\\Coursework_sem5_sounnds";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        if (_loadFile(selectedFilePath)) {
            std::wstring fileName = GetFileNameFromPath(ofn.lpstrFile);
            SetWindowText(hEditFileName, fileName.data());
            std::wstring s = L"Номер текущего второго канала: ";
            if (audioPlayerChannelsOriginal.size() > 0) {
                s += std::to_wstring(currFirstChannel + 2);
                SetWindowText(hLabelSecondChannel, s.c_str());
            }
            else {
                SetWindowText(hLabelSecondChannel, s.c_str());
            }
            s = L"Номер текущего первого канала: ";
            if (audioPlayerChannelsOriginal.size() > 0) {
                s += std::to_wstring(currFirstChannel + 1);
                SetWindowText(hLabelFirtChannel, s.c_str());
            }
            else {
                SetWindowText(hLabelFirtChannel, s.c_str());
            }
            RECT rect;
            GetClientRect(hwnd, &rect);
            rect.top += rect.bottom / 4;
            rect.bottom -= rect.bottom / 4;
            waveformRendererSetNull(w);
            InvalidateRect(hwnd, &rect, FALSE);
        }
        else {
            MessageBox(NULL, L"Не поддерживаемый файл", L"Ошибка", MB_OK | MB_ICONERROR);
        }
    }
}
/*std::pair<vector<int16_t>, int> resample(const std::vector<int16_t>& inputSamples, int originalSampleRate, int targetSampleRate) {
    float resampleRatio = static_cast<float>(originalSampleRate) / static_cast<float>(targetSampleRate);
    size_t newSampleCount = static_cast<size_t>(inputSamples.size() * resampleRatio);
    std::vector<int16_t> outputSamples(newSampleCount);
    for (size_t i = 0; i < newSampleCount; ++i) {
        float originalPosition = i / resampleRatio;
        size_t index1 = static_cast<size_t>(originalPosition);
        size_t index2 = index1 + 1;
        if (index2 < inputSamples.size()) {
            float fraction = originalPosition - index1;
            outputSamples[i] = inputSamples[index1] * (1 - fraction) + inputSamples[index2] * fraction;
        }
        else {
            outputSamples[i] = inputSamples[index1];
        }
    }

    return make_pair(outputSamples, targetSampleRate);
}*/

void DrawSelection(int width, int height, HWND hwnd, HDC hdc) {
    int left = selection->get_left();
    int right = selection->get_right();
    if (left == right)
        return;
    RECT rect;
    rect.left = selection->get_left() + START_X_POS;
    rect.right = selection->get_right() + START_X_POS;
    rect.top = height / 4;
    rect.bottom = height * 3 / 4;
    Buffer* copy = CreateScreenBuffer(hwnd);
    HDC hdcSource = screen_buffer->hdc;
    HDC hdcDest = copy->hdc;
    RefreshScreenBuffer(copy);
    BitBlt(hdcDest, 0, 0,
        copy->screenBufferRect.right - copy->screenBufferRect.left,
        copy->screenBufferRect.bottom - copy->screenBufferRect.top,
        hdcSource,
        0, 0,
        SRCCOPY);
    ModifyPixels(copy, rect, -50, -50, -50);
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    SelectObject(copy->hdc, hPen);
    MoveToEx(copy->hdc, START_X_POS, height / 4, NULL);
    LineTo(copy->hdc, width, height / 4);
    LineTo(copy->hdc, width, height * 3 / 4);
    LineTo(copy->hdc, START_X_POS, height * 3 / 4);
    LineTo(copy->hdc, START_X_POS, height / 4);
    DeleteObject(hPen);
    hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(copy->hdc, hPen);
    MoveToEx(copy->hdc, START_X_POS, height / 2, NULL);
    LineTo(copy->hdc, width, height / 2);
    DeleteObject(hPen);
    DrawScreenBuffer(copy, hdc);
    ReleaseScreenBuffer(copy);
    return;
}
void DrawButtonsBuffer(int buttonId) {
    Buffer* buffer = v_buffers.at(buttonId);
    FillRect(buffer->hdc, &buffer->screenBufferRect, (HBRUSH)CreateSolidBrush(RGB(230, 230, 255)));
    switch (buttonId) {
    case START:
        DrawStart(v_buffers.at(START)->hdc, RGB(0, 130, 0));
        break;
    case FINISH:
        DrawFinish(v_buffers.at(FINISH)->hdc);
        break;
    case PAUSE:
        DrawPause(v_buffers.at(PAUSE)->hdc);
        break;
    case MOVE_TO_END:
        DrawMoveToEnd(v_buffers.at(MOVE_TO_END)->hdc);
        break;
    case MOVE_TO_START:
        DrawMoveToStart(v_buffers.at(MOVE_TO_START)->hdc);
        break;
    case LOOP:
        DrawLoop(v_buffers.at(LOOP)->hdc);
        break;
    case RECORD:
        DrawRecord(v_buffers.at(RECORD)->hdc);
        break;
    default:
        break;
    }
}
void DrawButtonFunc(bool flag, HWND hWnd, Buffer* buffer, int x, int y, int incRed, int incGreen, int incBlue) {
    Buffer* buf = buffer;
    if (flag) {
        buf = CreateScreenBuffer(hWnd, { 0, 0, 50, 50 });
        DrawScreenBuffer(buffer, buf->hdc);
        ModifyPixels(buf, { 0, 0, 50, 50 }, incRed, incGreen, incBlue);
    }

    HDC hdc = GetDC(hWnd);
    BitBlt(hdc, x, y,
        buf->screenBufferRect.right - buf->screenBufferRect.left,
        buf->screenBufferRect.bottom - buf->screenBufferRect.top,
        buf->hdc, 0, 0, SRCCOPY
    );
    if (flag)
        ReleaseScreenBuffer(buf);
    ReleaseDC(hWnd, hdc);
}
void DrawCustomButton(HWND hWnd, Buffer* buffer, int x, int y, UINT state, int additionalPar) {
    if (additionalPar == RECORD || additionalPar == LOOP || additionalPar == PAUSE
        /* || additionalPar == START || additionalPar == FINISH*/) {
        if (state == 17 || isRepaint) {
            switch (additionalPar) {
            case RECORD: {
                RecordCondition = !RecordCondition;
                DrawButtonFunc(RecordCondition, hWnd, buffer, x, y, 0, 0, 10);
                return;
            }
            case LOOP: {
                LoopCondition = !LoopCondition;
                DrawButtonFunc(LoopCondition, hWnd, buffer, x, y, 0, 0, 10);
                return;
            }
            case PAUSE: {
                //Logger::Log("Pause condition111: " + std::to_string(PauseCondition));
                PauseCondition = !PauseCondition;
                FillRect(buffer->hdc, &buffer->screenBufferRect, (HBRUSH)CreateSolidBrush(RGB(230, 230, 255)));
                if (!PauseCondition) {
                    //Logger::Log("Draw Pause on pause");
                    DrawPause(buffer->hdc);
                }
                else {
                    //Logger::Log("Draw Start on pause");
                    DrawStart(buffer->hdc, RGB(204, 85, 0));
                }
                DrawButtonFunc(PauseCondition, hWnd, buffer, x, y, 0, 0, 0);
                return;
            }
            default:
                break;
            }
        }
        else
            return;
    }
    
    DrawButtonFunc(state == 17, hWnd, buffer, x, y, 35, 35, 35);
}
std::vector<char> get_new_data(int typeSize) {
    if (audioPlayerChannels.size() > 0) {
        std::vector<char> data;
        int sz = audioPlayerChannels.at(0)->size();
        data.reserve(sz * audioPlayerChannels.size());
        for (int i = 0; i < sz; i += typeSize) {
            for (int j = 0; j < audioPlayerChannels.size(); j++) {
                for (int k = i; k < i + typeSize && k < sz; k++) {
                    data.push_back(audioPlayerChannels.at(j)->at(k));
                }
            }
        }
        return data;
    }
    return std::vector<char>();
}
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    //static HWND hWavePanel;  // Контейнер для звуковой волны
    //std::wstringstream ss;
    
    switch (msg) {
    case WM_UPDATE_RECORDED_AUDIO: {
        if (audio_player) {
            std::vector<char> recordedData(audio_player->getRecordedData());
            audio_player->set_isPaintIsDone(false);
            audio_player->MessageIsGotten = true;
            int numChannels = waveFormat.Format.nChannels;
            int size = recordedData.size() / numChannels;
            
            if (audioPlayerChannels.size() != 0) {
                audio_editorClear(audio_editor);
                for (auto ptr : audio_editorCh) {
                    if (ptr)
                        delete ptr;
                }
                audio_editorCh.clear();
                for (auto ptr : audio_editorChOrig) {
                    if (ptr)
                        delete ptr;
                }
                audio_editorChOrig.clear();
                points.clear();
                //audioChannelsClear(audioChannels);
                waveformRendererClear(w);
            }
            else {
                currFirstChannel = 0;
                audioEncodingConverter->set_currentType(currentType);
                audioEncodingConverter->set_origType(currentType);
                audioEncodingConverter->set_typeSize(_typeSize);
                audioEncodingConverter->set_origTypeSize(_typeSize);

                audioPlayerChannelsOriginal.reserve(numChannels);
                audioPlayerChannelsOriginal.resize(numChannels);
                audioPlayerChannels.reserve(numChannels);
                audioPlayerChannels.resize(numChannels);
                for (int i = 0; i < audioPlayerChannels.size(); i++) {
                    audioPlayerChannels.at(i) = new std::vector<char>();
                    audioPlayerChannelsOriginal.at(i) = new std::vector<char>();
                }
            }
            audio_editor.reserve(numChannels);
            audio_editor.resize(numChannels);
            audio_editorCh.reserve(numChannels);
            audio_editorCh.resize(numChannels);
            audio_editorChOrig.reserve(numChannels);
            audio_editorChOrig.resize(numChannels);
            points.reserve(numChannels);
            points.resize(numChannels);
            audioChannels.reserve(numChannels);
            audioChannels.resize(numChannels);
            std::vector<std::vector<char>*> rDataCopy(numChannels);
            std::vector<std::vector<char>*> rData(numChannels);
            std::vector<AudioDataVariant> rChannels(numChannels);
            bool b = false;
            for (int i = 0; i < rData.size(); i++) {
                rData.at(i) = new std::vector<char>();
                rData.at(i)->reserve(size);
                for (int j = 0; j < recordedData.size(); j += _typeSize * numChannels) {
                    for (int k = 0; k < _typeSize; k++) {
                        rData.at(i)->push_back(recordedData.at(j + k + i * _typeSize));
                    }
                }
                rDataCopy.at(i) = new vector<char>(*rData.at(i));
                std::visit([&](auto&& destVec) {
                    if (destVec == NULL)
                        b = true;
                    }, audioChannels.at(i));
                if (b)
                    setAudioDataByType(audioChannels.at(i), *rData.at(i), currentType);
                else
                    setAudioDataByType(rChannels.at(i), *rData.at(i), currentType);
            }
            uint8_t origType = audioEncodingConverter->get_origType();
            uint8_t origTypeSize = audioEncodingConverter->get_origTypeSize();
            uint8_t typeSize = audioEncodingConverter->get_typeSize();
            uint8_t type = audioEncodingConverter->get_currentType();

            audioEncodingConverter->set_basicData(rData);
            audioEncodingConverter->set_origType(type);
            audioEncodingConverter->set_origTypeSize(typeSize);
            audioEncodingConverter->set_currentType(origType);
            audioEncodingConverter->set_typeSize(origTypeSize);

            audioEncodingConverter->changeEncoding(origType, rDataCopy);

            for (int i = 0; i < numChannels; i++) {
                if (!b)
                    insertAudioDataVariant(audioChannels.at(i), rChannels.at(i));
                audioPlayerChannels.at(i)->insert(audioPlayerChannels.at(i)->end(), rData.at(i)->begin(), rData.at(i)->end());
                audioPlayerChannelsOriginal.at(i)->insert(audioPlayerChannelsOriginal.at(i)->end(), rDataCopy.at(i)->begin(), rDataCopy.at(i)->end());
            }
            audioEncodingConverter->set_basicData(audioPlayerChannelsOriginal);
            audioEncodingConverter->set_origType(origType);
            audioEncodingConverter->set_origTypeSize(origTypeSize);
            audioEncodingConverter->set_currentType(type);
            audioEncodingConverter->set_typeSize(typeSize);

            waveformRendererSetNull(w);
            firstRun = true;
            audio_player->set_audioData(get_new_data(_typeSize));
            //audio_player->set_start_position(0);
            //audio_player->set_end_position(audio_player->get_audioData().size() - 1);*/
            RECT rect;
            GetClientRect(hwnd, &rect);
            rect.top += rect.bottom / 4;
            rect.bottom -= rect.bottom / 4;
            rect.right -= 225;
            InvalidateRect(hwnd, &rect, FALSE);
        }
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;       
        HWND hStatic = (HWND)lParam;      
        if (hStatic == hLabelSettings) {  
            SetBkMode(hdcStatic, TRANSPARENT);     
            SetTextColor(hdcStatic, RGB(0, 0, 0)); 
            return (LRESULT)CreateSolidBrush(RGB(184, 215, 255));
        }
        else {
            SetBkMode(hdcStatic, TRANSPARENT);   
            SetTextColor(hdcStatic, RGB(0, 0, 0));
            return (LRESULT)CreateSolidBrush(RGB(255, 255, 255));
        }
        break;
    }
    case WM_CREATE: {
        audio_player = new AudioPlayer(hwnd);
        //const char* filePath = "sample-3s_float.wav";//"Able to play\\sine_wave_s_16bit.wav";  // Замените на путь к вашему файлу
        //const wchar_t* wfilePath = L"sample-3s_float.wav";//"Able to play\\sine_wave_s_16bit.wav";  // Замените на путь к вашему файлу
        //_loadFile(wfilePath);
       //std::vector<int16_t> samples = loadSamples(filePath);
       // audioData.clear();
       // for (int i = 0; i < samples.size(); i++) {
       //     audioData.push_back((int16_t)samples.at(i));
       // }
        //CreateWindow(L"BUTTON", L"Load File", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 50, 50, 100, 50, hwnd, (HMENU)1, NULL, NULL);
        hButtonPlay = CreateWindow(L"BUTTON", L"", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, START_X_POS + 60 * START, 10, 50, 50, hwnd, (HMENU)START, NULL, NULL);
        hButtonPause_Continue = CreateWindow(L"BUTTON", L"", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, START_X_POS + 60 * PAUSE, 10, 50, 50, hwnd, (HMENU)PAUSE, NULL, NULL);
        hButtonMoveToEnd = CreateWindow(L"BUTTON", L"", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, START_X_POS + 60 * MOVE_TO_END, 10, 50, 50, hwnd, (HMENU)MOVE_TO_END, NULL, NULL);
        hButtonMoveToStart = CreateWindow(L"BUTTON", L"", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, START_X_POS + 60 * MOVE_TO_START, 10, 50, 50, hwnd, (HMENU)MOVE_TO_START, NULL, NULL);
        hButtonCyclePlaying = CreateWindow(L"BUTTON", L"", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, START_X_POS + 60 * LOOP, 10, 50, 50, hwnd, (HMENU)LOOP, NULL, NULL);
        hButtonStop = CreateWindow(L"BUTTON", L"", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, START_X_POS + 60 * FINISH, 10, 50, 50, hwnd, (HMENU)FINISH, NULL, NULL);
        hButtonRecord = CreateWindow(L"BUTTON", L"", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, START_X_POS + 60 * RECORD, 10, 50, 50, hwnd, (HMENU)RECORD, NULL, NULL);
        hButtonMoveDown = CreateWindow(L"BUTTON", L"\\/", WS_VISIBLE | WS_CHILD, START_X_POS, 420, 20, 20, hwnd, (HMENU)7, NULL, NULL);
        hButtonMoveUp = CreateWindow(L"BUTTON", L"/\\", WS_VISIBLE | WS_CHILD, START_X_POS, 120, 20, 20, hwnd, (HMENU)8, NULL, NULL);
        hButtonLoadFile = CreateWindow(L"BUTTON", L"Загрузить файл", WS_VISIBLE | WS_CHILD, 430, 10, 150, 22, hwnd, (HMENU)9, NULL, NULL);
        hButtonUnloadData = CreateWindow(L"BUTTON", L"Выгрузить данные", WS_VISIBLE | WS_CHILD, 430, 10, 150, 22, hwnd, (HMENU)22, NULL, NULL);
        hButtonSaveFile = CreateWindow(L"BUTTON", L"Сохранить файл", WS_VISIBLE | WS_CHILD, 430, 10, 150, 22, hwnd, (HMENU)17, NULL, NULL);       
        hButtonApplySettings = CreateWindow(L"BUTTON", L"Применить настройки", WS_VISIBLE | WS_CHILD, 430, 10, 200, 25, hwnd, (HMENU)18, NULL, NULL);
        hComboBoxEncodingChoice = CreateWindowEx(0, L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 500, 100, 200, 200, hwnd, (HMENU)10, GetModuleHandle(NULL), NULL);
        SendMessage(hComboBoxEncodingChoice, CB_ADDSTRING, 0, (LPARAM)L"unsigned 8-bit");
        SendMessage(hComboBoxEncodingChoice, CB_ADDSTRING, 0, (LPARAM)L"signed 16-bit");
        SendMessage(hComboBoxEncodingChoice, CB_ADDSTRING, 0, (LPARAM)L"signed 24-bit");
        SendMessage(hComboBoxEncodingChoice, CB_ADDSTRING, 0, (LPARAM)L"signed 32-bit");
        SendMessage(hComboBoxEncodingChoice, CB_ADDSTRING, 0, (LPARAM)L"IEEE float");
        SendMessage(hComboBoxEncodingChoice, CB_SETCURSEL, 1, 0);
        hComboBoxDiscretisationChoice = CreateWindowEx(0, L"COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 500, 100, 400, 400, hwnd, (HMENU)14, GetModuleHandle(NULL), NULL);
        SendMessage(hComboBoxDiscretisationChoice, CB_ADDSTRING, 0, (LPARAM)L"8000");
        SendMessage(hComboBoxDiscretisationChoice, CB_ADDSTRING, 0, (LPARAM)L"11025");
        SendMessage(hComboBoxDiscretisationChoice, CB_ADDSTRING, 0, (LPARAM)L"16000");
        SendMessage(hComboBoxDiscretisationChoice, CB_ADDSTRING, 0, (LPARAM)L"22050");
        SendMessage(hComboBoxDiscretisationChoice, CB_ADDSTRING, 0, (LPARAM)L"32000");
        SendMessage(hComboBoxDiscretisationChoice, CB_ADDSTRING, 0, (LPARAM)L"44100");
        SendMessage(hComboBoxDiscretisationChoice, CB_ADDSTRING, 0, (LPARAM)L"48000");
        SendMessage(hComboBoxDiscretisationChoice, CB_ADDSTRING, 0, (LPARAM)L"88200");
        SendMessage(hComboBoxDiscretisationChoice, CB_ADDSTRING, 0, (LPARAM)L"96000");
        SendMessage(hComboBoxDiscretisationChoice, CB_ADDSTRING, 0, (LPARAM)L"176400");
        SendMessage(hComboBoxDiscretisationChoice, CB_ADDSTRING, 0, (LPARAM)L"192000");
        //SendMessage(hComboBoxDiscretisationChoice, CB_ADDSTRING, 0, (LPARAM)L"352800");
        //SendMessage(hComboBoxDiscretisationChoice, CB_ADDSTRING, 0, (LPARAM)L"384000");
        SendMessage(hComboBoxDiscretisationChoice, CB_SETCURSEL, 5, 0);
        hLabelSettings = CreateWindowEx(0, L"STATIC", L"               Настройки:", WS_CHILD | WS_VISIBLE, 50, 50, 200, 25, hwnd, (HMENU)11, GetModuleHandle(NULL), NULL);
        hLabelEncoding = CreateWindowEx(0, L"STATIC", L"Кодировка:", WS_CHILD | WS_VISIBLE, 50, 100, 200, 25, hwnd, (HMENU)12, GetModuleHandle(NULL), NULL);
        hLabelSpeed = CreateWindowEx(0, L"STATIC", L"Скорость:", WS_CHILD | WS_VISIBLE, 50, 150, 200, 25, hwnd, (HMENU)13, GetModuleHandle(NULL), NULL);
        hLabelFirtChannel = CreateWindowEx(0, L"STATIC", L"Номер текущего первого канала:", WS_CHILD | WS_VISIBLE, 50, 150, 200, 25, hwnd, (HMENU)13, GetModuleHandle(NULL), NULL);
        hLabelSecondChannel = CreateWindowEx(0, L"STATIC", L"Номер текущего второго канала:", WS_CHILD | WS_VISIBLE, 50, 150, 200, 25, hwnd, (HMENU)13, GetModuleHandle(NULL), NULL);
        hLabelDiscretisation = CreateWindowEx(0, L"STATIC", L"Дискретизация:", WS_CHILD | WS_VISIBLE, 50, 150, 200, 25, hwnd, (HMENU)14, GetModuleHandle(NULL), NULL);
        hEditSpinBox = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"1", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT, 50, 50, 100, 25, hwnd, (HMENU)15, GetModuleHandle(NULL), NULL);
        hEditSpinBoxRecord = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"1", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT, 50, 50, 100, 25, hwnd, (HMENU)20, GetModuleHandle(NULL), NULL);
        hEditFileName = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT, 50, 50, 100, 25, hwnd, (HMENU)19, GetModuleHandle(NULL), NULL);

        hSpinBoxUpDown = CreateWindowEx(0, UPDOWN_CLASS, NULL, WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_SETBUDDYINT, 0, 0, 0, 0, hwnd, (HMENU)16, GetModuleHandle(NULL), NULL);
        SendMessage(hSpinBoxUpDown, UDM_SETBUDDY, (WPARAM)hEditSpinBox, 0);
        UDACCEL accel = { 0, static_cast<UINT>(STEP * -100) };
        SendMessage(hSpinBoxUpDown, UDM_SETACCEL, 1, reinterpret_cast<LPARAM>(&accel));

        hSpinBoxUpDownRecord = CreateWindowEx(0, UPDOWN_CLASS, NULL, WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_SETBUDDYINT, 0, 0, 0, 0, hwnd, (HMENU)21, GetModuleHandle(NULL), NULL);
        SendMessage(hSpinBoxUpDownRecord, UDM_SETBUDDY, (WPARAM)hEditSpinBoxRecord, 0);
        SendMessage(hSpinBoxUpDownRecord, UDM_SETRANGE32, (WPARAM)(1), (LPARAM)(2));
        UDACCEL accelRecord = { 0, static_cast<UINT>(1) };
        SendMessage(hSpinBoxUpDownRecord, UDM_SETACCEL, 1, reinterpret_cast<LPARAM>(&accelRecord));

        screen_buffer = CreateScreenBuffer(hwnd);

        // Настройка прокрутки
        SCROLLINFO si = { sizeof(si), SIF_RANGE | SIF_PAGE, 0, 1000, 10, 0 };
        //SetScrollInfo(hWavePanel, SB_HORZ, &si, TRUE);
        //SetScrollInfo(hWavePanel, SB_VERT, &si, TRUE);
        break;
    }
    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
        int state = 0;
        if (pDIS != NULL)
            state = pDIS->itemState;
        if (isRepaint) {
            //LoopCondition = true;
            //RecordCondition = true;
            //PauseCondition = true;
            cnt++;
        }
    
        switch (pDIS->CtlID) {
        case START:
            //Logger::Log("DRAW START");
            DrawCustomButton(hwnd, v_buffers.at(START), START_X_POS + 60 * START, 10, state, START);
            if (state == 17) {
                PauseCondition = true;
                DrawCustomButton(hwnd, v_buffers.at(PAUSE), START_X_POS + 60 * PAUSE, 10, state, PAUSE);
            }
            break;
        case PAUSE:
            //Logger::Log("DRAW PAUSE");
            if (state == 17 || isRepaint) {
                DrawCustomButton(hwnd, v_buffers.at(PAUSE), START_X_POS + 60 * PAUSE, 10, state, PAUSE);
            }
            break;
        case FINISH:
            //Logger::Log("DRAW FINISH");
            DrawCustomButton(hwnd, v_buffers.at(FINISH), START_X_POS + 60 * FINISH, 10, state, FINISH);
            if (state == 17) {
                PauseCondition = true;
                DrawCustomButton(hwnd, v_buffers.at(PAUSE), START_X_POS + 60 * PAUSE, 10, state, PAUSE);
            }
            break;
        case MOVE_TO_START:
            //Logger::Log("MOVE_TO_START");
            DrawCustomButton(hwnd, v_buffers.at(MOVE_TO_START), START_X_POS + 60 * MOVE_TO_START, 10, state, MOVE_TO_START);
            break;
        case MOVE_TO_END:
            DrawCustomButton(hwnd, v_buffers.at(MOVE_TO_END), START_X_POS + 60 * MOVE_TO_END, 10, state, MOVE_TO_END);
            break;
        case LOOP:
            if (state == 17 || isRepaint) {
                DrawCustomButton(hwnd, v_buffers.at(LOOP), START_X_POS + 60 * LOOP, 10, state, LOOP);
            }
            break;
        case RECORD:
            if (state == 17 || isRepaint) {
                DrawCustomButton(hwnd, v_buffers.at(RECORD), START_X_POS + 60 * RECORD, 10, state, RECORD);
            }
            break;
        default:
            break;
        }
        if (isRepaint && cnt == 7) {
            isRepaint = false;
        }
        break;
    }
    case WM_HSCROLL: {
        SCROLLINFO si;
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_HORZ, &si);

        int scrollPos = si.nPos;
        switch (LOWORD(wParam)) {
        case SB_LINELEFT:   // Прокрутка влево
            scrollPos -= 1;
            break;
        case SB_LINERIGHT:  // Прокрутка вправо
            scrollPos += 1;
            break;
        case SB_PAGELEFT:   // Прокрутка на одну страницу влево
            scrollPos -= si.nPage;
            break;
        case SB_PAGERIGHT:  // Прокрутка на одну страницу вправо
            scrollPos += si.nPage;
            break;
        case SB_THUMBTRACK: // Прокрутка по положению ползунка
            scrollPos = HIWORD(wParam);
            break;
        }

        // Обновляем позицию прокрутки
        si.fMask = SIF_POS;
        si.nPos = scrollPos;
        SetScrollInfo(hwnd, SB_HORZ, &si, FALSE);

        // Смещение для отрисовки
        offset = scrollPos * 1000;
        InvalidateRect(hwnd, NULL, FALSE);  // Перерисовка окна
    }
                   break;

    case WM_VSCROLL: {
        // По аналогии с горизонтальной прокруткой (SB_VERT)
    }
    break;
    case WM_KEYDOWN: {
        RECT rect;
        GetClientRect(hwnd, &rect);
        rect.top += rect.bottom / 4;
        rect.bottom -= rect.bottom / 4;
        rect.right -= 225;

        switch (wParam) {
        case VK_UP:
            if (selection != NULL && !selection->get_isActive()) {
                //Logger::Log("VK_UP");
                scale *= 0.8f;
                InvalidateRect(hwnd, &rect, FALSE);
            }
            break;
        case VK_DOWN:
            if (selection != NULL && !selection->get_isActive()) {
                //Logger::Log("VK_DOWN");
                scale *= 1.25f;
                InvalidateRect(hwnd, &rect, FALSE);
            }
            break;
        case VK_LEFT:
            if (selection != NULL && !selection->get_isActive()) {
                //Logger::Log("VK_LEFT");
                processOffsetUpdate(scale, rect, w, true);
                InvalidateRect(hwnd, &rect, FALSE);
            }
            break;
        case VK_RIGHT:
            if (selection != NULL && !selection->get_isActive()) {
                //Logger::Log("VK_RIGHT");
                processOffsetUpdate(scale, rect, w, false);
                InvalidateRect(hwnd, &rect, FALSE);
            }
            break;
        case 'C':
            if (GetKeyState(VK_CONTROL) & 0x8000) {
                if (selection != nullptr) {
                    //Logger::Log("CONTROL C");
                    if (selection->get_isActive() && !selection->get_isDown()) {
                        for (int i = 0; i < audioChannels.size(); i++) {
                            _copy_part_of_Audio(selection, audio_editor.at(i), w);
                            __copy_part_of_Audio(currentType, selection, audio_editorCh.at(i), w);
                            __copy_part_of_Audio(audioEncodingConverter->get_origType(), selection, audio_editorChOrig.at(i), w);
                        }
                    }
                }
                InvalidateRect(hwnd, &rect, FALSE);
            }
            break;
        case 'Z':
            //Logger::Log("__Z");
            if ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000)) {
                ///Logger::Log("_SHIFT CONTROL Z");
                if (selection != nullptr) {
                    //Logger::Log("SHIFT CONTROL Z");
                    for (int i = 0; i < audioChannels.size(); i++) {
                        processEditorAction(selection, audio_editor.at(i), w, audioChannels.at(i), rect.right, _undo);
                        processEditorAction(currentType, selection, audio_editorCh.at(i), w, audioPlayerChannels.at(i), rect.right, _undo);
                        processEditorAction(audioEncodingConverter->get_origType(), selection, audio_editorChOrig.at(i), w, audioPlayerChannelsOriginal.at(i), rect.right, _undo);
                    }
                    audio_player->set_audioData(get_new_data(_typeSize));
                    audioEncodingConverter->set_basicData(audioPlayerChannelsOriginal);
                }
            }
            else {
                if (GetKeyState(VK_CONTROL) & 0x8000) {
                    //Logger::Log("_CONTROL Z");
                    if (selection != nullptr) {
                        //Logger::Log("CONTROL Z");
                        for (int i = 0; i < audioChannels.size(); i++) {
                            processEditorAction(selection, audio_editor.at(i), w, audioChannels.at(i), rect.right, _cancel);
                            processEditorAction(currentType, selection, audio_editorCh.at(i), w, audioPlayerChannels.at(i), rect.right, _cancel);
                            processEditorAction(audioEncodingConverter->get_origType(), selection, audio_editorChOrig.at(i), w, audioPlayerChannelsOriginal.at(i), rect.right, _cancel);
                        }
                        audio_player->set_audioData(get_new_data(_typeSize));
                        audioEncodingConverter->set_basicData(audioPlayerChannelsOriginal);
                    }
                }
            }
            InvalidateRect(hwnd, &rect, FALSE);
            break;
        case 'V':
            if (GetKeyState(VK_CONTROL) & 0x8000) {
                //Logger::Log("__CONTROL X");
                if (selection != nullptr) {
                    //Logger::Log("_CONTROL X");
                    if (!selection->get_isActive()) {
                        //Logger::Log("CONTROL V");
                        for (int i = 0; i < audioChannels.size(); i++) {
                            processEditorAction(selection, audio_editor.at(i), w, audioChannels.at(i), rect.right, _insert);
                            processEditorAction(currentType, selection, audio_editorCh.at(i), w, audioPlayerChannels.at(i), rect.right, _insert);
                            processEditorAction(audioEncodingConverter->get_origType(), selection, audio_editorChOrig.at(i), w, audioPlayerChannelsOriginal.at(i), rect.right, _insert);
                        }
                        audio_player->set_audioData(get_new_data(_typeSize));
                        audioEncodingConverter->set_basicData(audioPlayerChannelsOriginal);
                    }
                }
            }
            InvalidateRect(hwnd, &rect, FALSE);
            break;

        case 'X':
            //Logger::Log("__CONTROL X");
            if (selection != nullptr) {
                //Logger::Log("_CONTROL X");
                if (selection->get_isActive() && !selection->get_isDown()) {
                    //Logger::Log("CONTROL X");
                    for (int i = 0; i < audioChannels.size(); i++) {
                        processEditorAction(selection, audio_editor.at(i), w, audioChannels.at(i), rect.right, _cut);
                        processEditorAction(currentType, selection, audio_editorCh.at(i), w, audioPlayerChannels.at(i), rect.right, _cut);
                        processEditorAction(audioEncodingConverter->get_origType(), selection, audio_editorChOrig.at(i), w, audioPlayerChannelsOriginal.at(i), rect.right, _cut);
                    }
                    audio_player->set_audioData(get_new_data(_typeSize));
                    audioEncodingConverter->set_basicData(audioPlayerChannelsOriginal);
                }
            }
            selection->set_isActive(false);
            InvalidateRect(hwnd, &rect, FALSE);
            break;

        case VK_DELETE:
        case VK_BACK:
            //Logger::Log("__DELETE");
            if (selection != nullptr) {
                //Logger::Log("_DELETE");
                if (selection->get_isActive() && !selection->get_isDown()) {
                    if (GetKeyState(VK_CONTROL) & 0x8000) {
                        //Logger::Log("CONTROL DELETE");
                        for (int i = 0; i < audioChannels.size(); i++) {
                            processEditorAction(selection, audio_editor.at(i), w, audioChannels.at(i), rect.right, _delete_all_except_of_part);
                            processEditorAction(currentType, selection, audio_editorCh.at(i), w, audioPlayerChannels.at(i), rect.right, _delete_all_except_of_part);
                            processEditorAction(audioEncodingConverter->get_origType(), selection, audio_editorChOrig.at(i), w, audioPlayerChannelsOriginal.at(i), rect.right, _delete_all_except_of_part);
                        }
                        audio_player->set_audioData(get_new_data(_typeSize));
                        audioEncodingConverter->set_basicData(audioPlayerChannelsOriginal);
                    }
                    else {
                        //Logger::Log("DELETE");
                        for (int i = 0; i < audioChannels.size(); i++) {
                            processEditorAction(selection, audio_editor.at(i), w, audioChannels.at(i), rect.right, _delete_part);
                            processEditorAction(currentType, selection, audio_editorCh.at(i), w, audioPlayerChannels.at(i), rect.right, _delete_part);
                            processEditorAction(audioEncodingConverter->get_origType(), selection, audio_editorChOrig.at(i), w, audioPlayerChannelsOriginal.at(i), rect.right, _delete_part);
                        }
                        audio_player->set_audioData(get_new_data(_typeSize));
                        audioEncodingConverter->set_basicData(audioPlayerChannelsOriginal);
                    }
                }
            }
            selection->set_isActive(false);
            InvalidateRect(hwnd, &rect, FALSE);
            break;
        default:
            break;
        }
        break;
    }
    case WM_COMMAND: {
        RECT rect;
        GetClientRect(hwnd, &rect);
        rect.top += rect.bottom / 4;
        rect.bottom -= rect.bottom / 4;
        rect.right -= 225;
        int wparamL = LOWORD(wParam);
        int wparamH = HIWORD(wParam);
        switch (wParam) {
        case START:
            if (!audio_player->moveEndCalled && !audio_player->moveStartCalled) {
                setStartAndEndPosition(audioPlayerChannels.size(), currentType, scale, (*selection), audio_player, w);
            }
            audio_player->play();
            SetFocus(hwnd);
            break;
        case FINISH:
            audio_player->stop();
            SetFocus(hwnd);
            break;
        case PAUSE:
            if (!audio_player->get_isPaused())
                audio_player->pause();
            else
                audio_player->resume();
            if (!audio_player->get_isPausedRecording())
                audio_player->pauseRecording();
            else
                audio_player->resumeRecording();
            SetFocus(hwnd);
            break;
        case MOVE_TO_END:
            if (selection == nullptr)
                selection = new Selection();
            selection->set_start({ rect.right - 1, rect.top });
            selection->set_end(selection->get_start());
            selection->set_isDown(true);
            selection->set_isActive(false);
            isMove = true;
            audio_player->moveEndCalled = true;
            audio_player->move_to_end();
            InvalidateRect(hwnd, &rect, FALSE);
            SetFocus(hwnd);
            break;
        case MOVE_TO_START:
            if (selection == nullptr)
                selection = new Selection();
            selection->set_start({ rect.left + 1, rect.top });
            selection->set_end(selection->get_start());
            selection->set_isDown(true);
            selection->set_isActive(false);
            isMove = true;
            audio_player->moveStartCalled = true;
            audio_player->move_to_start();
            InvalidateRect(hwnd, &rect, FALSE);
            SetFocus(hwnd);
            break;
        case LOOP:
            audio_player->loop();
            SetFocus(hwnd);
            break;
        case RECORD:
            if (audioPlayerChannels.size() == 0) {
                wchar_t enc_buffer[256];
                GetWindowText(hComboBoxEncodingChoice, enc_buffer, 256);
                wstring targetEnc(enc_buffer);

                wchar_t dicr_buffer[256];
                GetWindowText(hComboBoxDiscretisationChoice, dicr_buffer, 256);
                wstring targetDiscr(dicr_buffer);
                int _targetDiscr = std::stoi(targetDiscr);
                int bitsPerSample = 0;
                if (targetEnc == L"unsigned 8-bit") {
                    currentType = _UINT8;
                    _typeSize = 1;
                    bitsPerSample = 8;
                }
                else {
                    if (targetEnc == L"signed 16-bit") {
                        currentType = _INT16;
                        _typeSize = 2;
                        bitsPerSample = 16;
                    }
                    else {
                        if (targetEnc == L"signed 24-bit") {
                            currentType = _INT24;
                            _typeSize = 3;
                            bitsPerSample = 24;
                        }
                        else {
                            if (targetEnc == L"signed 32-bit") {
                                currentType = _INT32;
                                _typeSize = 4;
                                bitsPerSample = 32;
                            }
                            else {
                                if (targetEnc == L"IEEE float") {
                                    currentType = _FLOAT;
                                    _typeSize = 4;
                                    bitsPerSample = 32;
                                }
                            }
                        }
                    }
                }
                wchar_t nChannels_buffer[256];
                GetWindowText(hEditSpinBoxRecord, nChannels_buffer, 256);
                wstring s_speed(nChannels_buffer);
                float nChannels;
                if (!tryParseFloat(s_speed, nChannels))
                    nChannels = 1;
                if (nChannels > 2)
                    nChannels = 2;
                if (nChannels < 1)
                    nChannels = 1;
                waveFormat.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE; // Используем расширенный формат
                waveFormat.Format.nChannels = (WORD)nChannels;             // Устанавливаем количество каналов
                waveFormat.Format.nSamplesPerSec = _targetDiscr;         // Частота дискретизации
                waveFormat.Format.wBitsPerSample = bitsPerSample;      // Количество бит на сэмпл
                waveFormat.Format.nBlockAlign = bitsPerSample / 8 * nChannels;            // Размер блока (байт на сэмпл)
                waveFormat.Format.nAvgBytesPerSec = waveFormat.Format.nBlockAlign * _targetDiscr;// Среднее количество байт в секунду
                waveFormat.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX); // Для WAVEFORMATEXTENSIBLE
                if (currentType == _FLOAT)
                    waveFormat.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
                else
                    waveFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
                waveFormat.Samples.wReserved = 0;
                waveFormat.Samples.wSamplesPerBlock = 0;
                waveFormat.Samples.wValidBitsPerSample = 0;
                waveFormat.dwChannelMask = 0;
                for (int i = 0; i < (int)nChannels; i++) {
                    waveFormat.dwChannelMask |= speakerChannels.at(i);
                }
                //audio_player->set_waveFormat(waveFormat);
            }
            else {
                
            }
            if (RecordCondition) {
                audio_player->set_waveFormat(waveFormat);
                if (waveFormat.Format.nChannels > 2) {
                    MessageBox(NULL, L"Запись с данным количеством каналов не поддерживается!", L"Информация", MB_OK | MB_ICONINFORMATION);
                    break;
                }
                audio_player->startRecording();
            }
            else {
                audio_player->stopRecording();
            }
            SetFocus(hwnd);
            break;
        case 7: {
            if (currFirstChannel > 0) {
                currFirstChannel--;
                std::wstring s = L"Номер текущего второго канала: ";
                if (audioPlayerChannelsOriginal.size() > 0) {
                    s += std::to_wstring(currFirstChannel + 2);
                    SetWindowText(hLabelSecondChannel, s.c_str());
                }
                else {
                    SetWindowText(hLabelSecondChannel, s.c_str());
                }
                s = L"Номер текущего первого канала: ";
                if (audioPlayerChannelsOriginal.size() > 0) {
                    s += std::to_wstring(currFirstChannel + 1);
                    SetWindowText(hLabelFirtChannel, s.c_str());
                }
                else {
                    SetWindowText(hLabelFirtChannel, s.c_str());
                }
                InvalidateRect(hwnd, &rect, FALSE);
            }
            SetFocus(hwnd);
            break;
        }
        case 8: {
            if (currFirstChannel < audioChannels.size() - 2) {
                currFirstChannel++;
                std::wstring s = L"Номер текущего второго канала: ";
                if (audioPlayerChannelsOriginal.size() > 0) {
                    s += std::to_wstring(currFirstChannel + 2);
                    SetWindowText(hLabelSecondChannel, s.c_str());
                }
                else {
                    SetWindowText(hLabelSecondChannel, s.c_str());
                }
                s = L"Номер текущего первого канала: ";
                if (audioPlayerChannelsOriginal.size() > 0) {
                    s += std::to_wstring(currFirstChannel + 1);
                    SetWindowText(hLabelFirtChannel, s.c_str());
                }
                else {
                    SetWindowText(hLabelFirtChannel, s.c_str());
                }
                InvalidateRect(hwnd, &rect, FALSE);
            }
            SetFocus(hwnd);
            break;
        }
        case 9: {
            if (audio_player->get_isPlaying()) {
                MessageBox(NULL, L"Идёт воспроизведение, загрузка невозможна!", L"Информация", MB_OK | MB_ICONINFORMATION);
                SetFocus(hwnd);
                break;
            }
            if (audio_player->get_isRecording()) {
                MessageBox(NULL, L"Идёт запись, загрузка невозможна!", L"Информация", MB_OK | MB_ICONINFORMATION);
                SetFocus(hwnd);
                break;
            }
            openFileDialog(hwnd);
            break;
        }
        case 17: {
            if (audioPlayerChannels.size() > 0) {               
                wchar_t name_buffer[256];
                GetWindowText(hEditFileName, name_buffer, 256);
                wstring fileName(name_buffer);
                vector<char> data = get_new_data(waveFormat.Format.wBitsPerSample / 8);
                saveWavFile(fileName, waveFormat, data);
            }
            else {
                MessageBox(NULL, L"Аудиоданные пусты!", L"Информация", MB_OK | MB_ICONINFORMATION);
            }
            break;
        }
        case 18: {
            if (audioPlayerChannels.size() > 0) {
                wchar_t enc_buffer[256];
                GetWindowText(hComboBoxEncodingChoice, enc_buffer, 256);
                wstring targetEnc(enc_buffer);  

                wchar_t dicr_buffer[256];
                GetWindowText(hComboBoxDiscretisationChoice, dicr_buffer, 256);
                wstring targetDiscr(dicr_buffer);
                int _targetDiscr = std::stoi(targetDiscr);

                wchar_t speed_buffer[256];
                GetWindowText(hEditSpinBox, speed_buffer, 256);
                wstring s_speed(speed_buffer);
                float speed;
                if (!tryParseFloat(s_speed, speed))
                    speed = 1;
                uint8_t targetType = -1;
                uint8_t typeSize = -1;
                if (targetEnc == L"unsigned 8-bit") {
                    targetType = _UINT8;
                    typeSize = 1;
                }
                else {
                    if (targetEnc == L"signed 16-bit") {
                        targetType = _INT16;
                        typeSize = 2;
                    }
                    else {
                        if (targetEnc == L"signed 24-bit") {
                            targetType = _INT24;
                            typeSize = 3;
                        }
                        else {
                            if (targetEnc == L"signed 32-bit") {
                                targetType = _INT32;
                                typeSize = 4;
                            }
                            else {
                                if (targetEnc == L"IEEE float") {
                                    targetType = _FLOAT;
                                    typeSize = 4;
                                }
                            }
                        }
                    }
                }
                int numChannels = audioPlayerChannels.size();
                audio_editorClear(audio_editor);
                audio_editor.reserve(numChannels);
                audio_editor.resize(numChannels);

                for (auto ptr : audio_editorCh) {
                    if (ptr)
                        delete ptr;
                }
                audio_editorCh.clear();
                audio_editorCh.reserve(numChannels);
                audio_editorCh.resize(numChannels);

                for (auto ptr : audio_editorChOrig) {
                    if (ptr)
                        delete ptr;
                }
                audio_editorChOrig.clear();
                audio_editorChOrig.reserve(numChannels);
                audio_editorChOrig.resize(numChannels);

                points.clear();
                points.reserve(numChannels);
                points.resize(numChannels);

                audioChannelsClear(audioChannels);
                audioChannels.reserve(numChannels);
                audioChannels.resize(numChannels);
               
                std::vector<std::vector<char>*> copy_audioPlayerChannels(audioPlayerChannels.size());
                for (int i = 0; i < audioPlayerChannels.size(); i++) {
                    uint8_t origType = audioEncodingConverter->get_origType();
                    *audioPlayerChannels.at(i) = resampleBySpeed(speed, *audioPlayerChannelsOriginal.at(i), origType);
                    copy_audioPlayerChannels.at(i) = new std::vector<char>(*audioPlayerChannels.at(i));
                }

                audioEncodingConverter->set_basicData(copy_audioPlayerChannels);
                audioEncodingConverter->changeEncoding(targetType, audioPlayerChannels);

                audioEncodingConverter->set_currentType(targetType);
                audioEncodingConverter->set_typeSize(typeSize);

                for (int i = 0; i < copy_audioPlayerChannels.size(); i++) {
                    if (copy_audioPlayerChannels.at(i)) {
                        delete copy_audioPlayerChannels.at(i);
                        copy_audioPlayerChannels.at(i) = NULL;
                    }
                }
                copy_audioPlayerChannels.clear();
                for (int i = 0; i < audioPlayerChannels.size(); i++) {
                    setAudioDataByType(audioChannels.at(i), *audioPlayerChannels.at(i), targetType);
                }
                waveFormat.Format.nSamplesPerSec = _targetDiscr;         // Частота дискретизации
                waveFormat.Format.wBitsPerSample = typeSize * 8;      // Количество бит на сэмпл
                waveFormat.Format.nBlockAlign = waveFormat.Format.nChannels * waveFormat.Format.wBitsPerSample / 8; // Размер блока (байт на сэмпл)
                waveFormat.Format.nAvgBytesPerSec = waveFormat.Format.nBlockAlign * _targetDiscr; // Среднее количество байт в секунду
                if (targetType == _FLOAT)
                    waveFormat.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
                else
                    waveFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
                waveformRendererClear(w);
                waveformRendererSetNull(w);
                firstRun = true;
                audio_player->set_audioData(get_new_data(typeSize));
                audio_player->set_waveFormat(waveFormat);
                audio_player->set_start_position(0);
                audio_player->set_end_position(audio_player->get_audioData().size() - 1);
                currentType = targetType;
                switch (currentType) {
                case _UINT8:
                    _typeSize = 1;
                    break;
                case _INT16:
                    _typeSize = 2;
                    break;
                case _INT24:
                    _typeSize = 3;
                    break;
                case _INT32:
                    _typeSize = 4;
                    break;
                case _FLOAT:
                    _typeSize = 4;
                    break;
                }
                InvalidateRect(hwnd, &rect, FALSE);
                MessageBox(NULL, L"Настройка завершена", L"Информация", MB_OK | MB_ICONINFORMATION);
            }
            else {
                MessageBox(NULL, L"Аудиоданные пусты!", L"Информация", MB_OK | MB_ICONINFORMATION);
            }
            break;
        }
        case 22: {
            if (audioPlayerChannelsOriginal.size() > 0) {
                currFirstChannel = 0;
                audio_editorClear(audio_editor);

                for (auto ptr : audio_editorCh) {
                    if (ptr)
                        delete ptr;
                }
                audio_editorCh.clear();

                for (auto ptr : audio_editorChOrig) {
                    if (ptr)
                        delete ptr;
                }
                audio_editorChOrig.clear();

                points.clear();

                audioChannelsClear(audioChannels);

                for (auto ptr : audioPlayerChannels) {
                    if (ptr)
                        delete ptr;
                }
                audioPlayerChannels.clear();

                for (auto ptr : audioPlayerChannelsOriginal) {
                    if (ptr)
                        delete ptr;
                }
                audioPlayerChannelsOriginal.clear();
                std::wstring s = L"Номер текущего второго канала: ";
                if (audioPlayerChannelsOriginal.size() > 0) {
                    s += std::to_wstring(currFirstChannel + 2);
                    SetWindowText(hLabelSecondChannel, s.c_str());
                }
                else {
                    SetWindowText(hLabelSecondChannel, s.c_str());
                }
                s = L"Номер текущего первого канала: ";
                if (audioPlayerChannelsOriginal.size() > 0) {
                    s += std::to_wstring(currFirstChannel + 1);
                    SetWindowText(hLabelFirtChannel, s.c_str());
                }
                else {
                    SetWindowText(hLabelFirtChannel, s.c_str());
                }
                InvalidateRect(hwnd, &rect, FALSE);
                MessageBox(NULL, L"Данные выгружены", L"Информация", MB_OK | MB_ICONINFORMATION);
            }
            else {
                MessageBox(NULL, L"Данные отсутствуют!", L"Информация", MB_OK | MB_ICONINFORMATION);
            }
        }
        default:
            break;
        }
        if (LOWORD(wParam) == 15) {
            if (HIWORD(wParam) == EN_CHANGE) {
                if (updating)
                    break;
                updating = true;
                double value = getEditValue(hEditSpinBox);
                if (value < MIN_VALUE || value > MAX_VALUE || std::to_wstring(value).length() > 4) {
                    updateEditValue(hEditSpinBox, value);
                }
                updating = false;
            }
        }
        break;
    }
    case WM_PAINT: {
        //Logger::Log("WM_PAINT");
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        width -= 230;
        int height = rect.bottom - rect.top;
        if (!audioChannels.empty() && (firstRun || !selection->get_isActive() || wm_size)) {
            //Logger::Log("PAINT");
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
            DeleteObject(hBrush);
            int size = getAudioDataSize(audioChannels.at(0));
            if (size != 0) {
                //vector<POINT> points;
                int _width = width - START_X_POS;
                if (waveformRendererIsNull(w)) {
                    scale = (float)size / (float)_width;
                    uint8_t tSize = audioEncodingConverter->get_typeSize();
                    createWRandAEd(_typeSize, tSize, currentType, scale, audioChannels, audioPlayerChannels, audioPlayerChannelsOriginal, height, _width, points, audio_editor, audio_editorCh, audio_editorChOrig, w);
                }
                POINT cursorPos;
                GetCursorPos(&cursorPos);
                ScreenToClient(hwnd, &cursorPos);
                int channelChoice = 0;
                processWaveformRendering(currFirstChannel, audioChannels, w, audio_editor, points, 0, height / 4, _width, height / 4 + height / 8, cursorPos, scale, START_X_POS);
                if (!waveformRendererIsNull(w) && !points.empty()) {
                    //Logger::Log("0");
                    RefreshScreenBuffer(screen_buffer);
                    processWaveformDrawing(currFirstChannel, w, points, hdc, START_X_POS, screen_buffer);
                }
                else {
                    if (!waveformRendererIsNull(w)) {
                        processScale(w, scale);
                    }
                }
                //bool isNotSelection = false;
                if ((!firstRun && selection->get_isDown()) || (!firstRun && wm_size)) {
                    //isNotSelection = true;
                    if (selection->get_left() + START_X_POS > rect.right - 230) {
                        selection->set_end({ rect.right - 230 - START_X_POS, rect.top });
                        selection->set_start(selection->get_end());
                    }
                    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
                    SelectObject(screen_buffer->hdc, hPen);

                    MoveToEx(screen_buffer->hdc, selection->get_left() + START_X_POS, height / 4, NULL);
                    LineTo(screen_buffer->hdc, selection->get_left() + START_X_POS, height * 3 / 4);

                    DeleteObject(hPen);

                    if (isMove) {
                        selection->set_isDown(false);
                        isMove = false;
                    }
                }
                    
                if (wm_size && selection->get_isActive() && !firstRun) {
                    //Logger::Log("1");
                    //RefreshScreenBuffer(screen_buffer);
                    if (selection->get_right() + START_X_POS > rect.right - 230) {
                        selection->set_end({ rect.right - 230 - START_X_POS, rect.top });
                        selection->set_start({ selection->get_left(), rect.top });
                    }
                    if (selection->get_left() == selection->get_right()) {
                        selection->set_isActive(false);
                    }
                    //Logger::Log("LEFT" + std::to_string(selection->get_left()));
                    //Logger::Log("RIGHT" + std::to_string(selection->get_right()));
                    DrawSelection(width, height, hwnd, hdc);
                    wm_size = false;
                }
                else {
                    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
                    SelectObject(screen_buffer->hdc, hPen);
                    MoveToEx(screen_buffer->hdc, START_X_POS, height / 4, NULL);
                    LineTo(screen_buffer->hdc, width, height / 4);
                    LineTo(screen_buffer->hdc, width, height * 3 / 4);
                    LineTo(screen_buffer->hdc, START_X_POS, height * 3 / 4);
                    LineTo(screen_buffer->hdc, START_X_POS, height / 4);
                    MoveToEx(screen_buffer->hdc, START_X_POS, height / 2, NULL);
                    LineTo(screen_buffer->hdc, width, height / 2);
                    DeleteObject(hPen);
                    //Logger::Log("2");
                    DrawScreenBuffer(screen_buffer, hdc);
                }
            }
            if (wm_size)
                wm_size = false;
            firstRun = false;
        }
        else {
            if (!firstRun && selection->get_isActive()) {
                ///Logger::Log("3");
                DrawSelection(width, height, hwnd, hdc);
            }
            else {
                //Logger::Log("4");
                RefreshScreenBuffer(screen_buffer);
                DrawScreenBuffer(screen_buffer, hdc);
            }
        }
        /*HPEN hPen = CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
        MoveToEx(hdc, 10, height / 4, NULL);
        LineTo(hdc, width, height / 4);
        LineTo(hdc, width, height * 3 / 4);
        LineTo(hdc, 10, height * 3 / 4);
        LineTo(hdc, 10, height / 4);
        MoveToEx(hdc, 10, height / 2, NULL);
        LineTo(hdc, width, height / 2);*/
        EndPaint(hwnd, &ps);
        audio_player->set_isPaintIsDone(true);
        break;
    }
    case WM_MOUSEMOVE: {
        RECT rect;
        GetClientRect(hwnd, &rect);
        bool isWidthGood = LOWORD(lParam) >= START_X_POS && LOWORD(lParam) <= (rect.right - 230);
        if (isWidthGood && selection != nullptr && selection->get_isDown()) {
            rect.top += rect.bottom / 4;
            rect.bottom -= rect.bottom / 4;
            rect.left += START_X_POS;
            rect.right -= 230;
            selection->set_end({ LOWORD(lParam) - START_X_POS, HIWORD(lParam) });
            selection->set_isActive(true);
            InvalidateRect(hwnd, &rect, FALSE);
        }
    }
                        break;
    case WM_SIZING:
    {
        const int minWidth = 670;
        const int minHeight = 500;

        RECT* pRect = reinterpret_cast<RECT*>(lParam);

        int width = pRect->right - pRect->left;
        int height = pRect->bottom - pRect->top;

        if (width < minWidth)
            pRect->right = pRect->left + minWidth;

        if (height < minHeight)
            pRect->bottom = pRect->top + minHeight;

    }
    break;
    case WM_SIZE: {
        //Logger::Log("WM_SIZE\n");
        isRepaint = true;
        cnt = 0;
        LoopCondition = !LoopCondition;
        RecordCondition = !RecordCondition;
        PauseCondition = !PauseCondition;
        /*UpdateWindow(hButtonPlay);
        UpdateWindow(hButtonCyclePlaying);
        UpdateWindow(hButtonMoveToEnd);
        UpdateWindow(hButtonMoveToStart);
        UpdateWindow(hButtonPause_Continue);
        UpdateWindow(hButtonRecord);
        UpdateWindow(hButtonStop);*/
        //if (!isInitialization) {
        //    isRepaint = false;
        //    cnt = 14;
        //}
        wm_size = true;
        RECT rect;
        GetClientRect(hwnd, &rect);
        int height = rect.bottom - rect.top;
        int width = rect.right - rect.left;
        /*if (selection->get_left() + START_X_POS > rect.right - 230) {

        }
        if (selection->get_right() + START_X_POS > rect.right - 230) {
            selection->set_start({ selection->get_left(), rect.top });
            selection->set_end({ rect.right - 230 - START_X_POS, rect.top });
        }*/
        MoveWindow(hButtonMoveUp, START_X_POS, height / 4 - 20, 20, 20, TRUE);           
        MoveWindow(hButtonMoveDown, START_X_POS, height * 3 / 4, 20, 20, TRUE);
        MoveWindow(hComboBoxEncodingChoice, width - 220, height / 4 + 40, 200, 200, TRUE);
        MoveWindow(hComboBoxDiscretisationChoice, width - 220, height / 4 + 85, 200, 260, TRUE);
        MoveWindow(hLabelSettings, width - 220, height / 4, 200, 20, TRUE);
        MoveWindow(hLabelEncoding, width - 220, height / 4 + 20, 200, 20, TRUE);
        MoveWindow(hLabelSpeed, width - 220, height / 4 + 110, 200, 20, TRUE);
        MoveWindow(hLabelDiscretisation, width - 220, height / 4 + 65, 200, 20, TRUE);
        MoveWindow(hButtonLoadFile, width - 220, 10, 200, 25, TRUE);
        MoveWindow(hButtonSaveFile, width - 220, 35, 200, 25, TRUE);
        MoveWindow(hButtonApplySettings, width - 220, height / 4 + 160, 200, 30, TRUE);
        MoveWindow(hButtonUnloadData, START_X_POS, height - 40, 200, 30, TRUE);
        MoveWindow(hEditFileName, width - 220, 65, 200, 25, TRUE);
        std::wstring s = L"Номер текущего второго канала: ";
        if (audioPlayerChannelsOriginal.size() > 0) {
            s += std::to_wstring(currFirstChannel + 2);
            SetWindowText(hLabelSecondChannel, s.c_str());
        }
        else {
            SetWindowText(hLabelSecondChannel, s.c_str());
        }
        s = L"Номер текущего первого канала: ";
        if (audioPlayerChannelsOriginal.size() > 0) {
            s += std::to_wstring(currFirstChannel + 1);
            SetWindowText(hLabelFirtChannel, s.c_str());
        }
        else {
            SetWindowText(hLabelFirtChannel, s.c_str());
        }
        MoveWindow(hLabelSecondChannel, 40, height * 3 / 4 + 5, 300, 20, TRUE);
        MoveWindow(hLabelFirtChannel, 40, height  / 4 - 22, 300, 20, TRUE);
        //MoveWindow(hEditSpinBox, width - 220, height / 4 + 130, 100, 25, TRUE);
        //MoveWindow(hSpinBoxUpDown, width - 220 + 80, height / 4 + 130, 20, 25, TRUE);
        // Перемещаем SpinBox с помощью SetWindowPos
        SetWindowPos(hEditSpinBoxRecord, HWND_TOP, 370, 65, 30, 25, SWP_NOZORDER);
        SetWindowPos(hSpinBoxUpDownRecord, HWND_TOP, 370 + 30, 65, 20, 25, SWP_NOZORDER);

        SetWindowPos(hEditSpinBox, HWND_TOP, width - 220, height / 4 + 130, 100, 25, SWP_NOZORDER);
        SetWindowPos(hSpinBoxUpDown, HWND_TOP, width - 220 + 100, height / 4 + 130, 20, 25, SWP_NOZORDER);

        ResizeScreenBuffer(&screen_buffer);
        //RefreshScreenBuffer(screen_buffer);
        //isInitialization = false;
        InvalidateRect(hwnd, NULL, FALSE);
        break;
    }
    case WM_LBUTTONDOWN: {
        SetFocus(hwnd);
        RECT rect;
        GetClientRect(hwnd, &rect);
        //int x = LOWORD(lParam);
        //int y = HIWORD(lParam);
        bool isWidthGood = LOWORD(lParam) >= START_X_POS && LOWORD(lParam) <= (rect.right - 230);
        bool isHeightGood = HIWORD(lParam) >= rect.bottom / 4 && HIWORD(lParam) <= rect.bottom * 3 / 4;
        if (isWidthGood && isHeightGood) {
            rect.top += rect.bottom / 4;
            rect.bottom -= rect.bottom / 4;
            rect.left += START_X_POS;
            rect.right -= 230;
            if (selection == nullptr)
                selection = new Selection();
            selection->set_start({ LOWORD(lParam) - START_X_POS, HIWORD(lParam) });
            selection->set_end(selection->get_start());
            selection->set_isDown(true);
            selection->set_isActive(false);
            audio_player->moveEndCalled = false;
            audio_player->moveStartCalled = false;
            InvalidateRect(hwnd, &rect, FALSE);
        }
    } break;
    case WM_LBUTTONUP: {
        selection->set_isDown(false);
        //InvalidateRect(hwnd, NULL, FALSE);
    }
                        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Основная функция WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    Logger::Init("thread_log.txt");
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = hInstance;
    //wc.style;
    wc.lpszClassName = L"WavVisualizerWindowClass";
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"Не удалось зарегистрировать класс окна!", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }
    //HBITMAP hBitmap = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_MYIMAGE), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"WAV Visualizer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }
    RECT rect;
    GetClientRect(hwnd, &rect);
    audioEncodingConverter = new AudioEncodingConverter();
    selection = new Selection(false, {rect.left, rect.top}, {rect.right - 230 - START_X_POS, rect.bottom});
    selection->set_isDown(false);
    rect = { 0, 0, 50, 50 };
    for (int i = 0; i < 7; i++) {
        v_buffers.push_back(CreateScreenBuffer(hwnd, rect));
        RefreshScreenBuffer(v_buffers.at(i));
        DrawButtonsBuffer(i);
    }
    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    waveformRendererClear(w);
    audio_editorClear(audio_editor);
    audioChannelsClear(audioChannels);

    for (auto ptr : audio_editorCh) {
        if (ptr)
            delete ptr;
    }
    audio_editorCh.clear();

    for (auto ptr : audioPlayerChannels) {
        if (ptr)
            delete ptr;
    }
    audioPlayerChannels.clear();

    for (auto ptr : audioPlayerChannelsOriginal) {
        if (ptr)
            delete ptr;
    }
    audioPlayerChannelsOriginal.clear();

    points.clear();

    if (selection)
        delete selection;
    if (screen_buffer)
        ReleaseScreenBuffer(screen_buffer);
    if (audio_player)
        delete audio_player;
    if (audioEncodingConverter)
        delete audioEncodingConverter;
    Logger::Close();
    return 0;
}
