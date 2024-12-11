#pragma once
#include <vector>
#include <variant>
#include <type_traits>

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
// Определение типа данных для хранения различных вариантов вектора
using AudioDataVariant = 
    std::variant<std::vector<float>*,
    std::vector<uint8_t>*,
    std::vector<int16_t>*,
    std::vector<int32_t>*>;

// Определение варианта для WaveformRenderer
using WaveformRendererVariant = std::variant<
    WaveformRenderer<float>*,
    WaveformRenderer<uint8_t>*,
    WaveformRenderer<int16_t>*,
    WaveformRenderer<int32_t>*>;

// Определение варианта для AudioEditor
using AudioEditorVariant = std::variant<
    AudioEditor<float>*,
    AudioEditor<uint8_t>*,
    AudioEditor<int16_t>*,
    AudioEditor<int32_t>*>;
void copyAudioData(const AudioDataVariant& source, AudioDataVariant& destination) {
    // Посещаем оба `std::variant` и копируем данные
    std::visit([&](auto* srcVec) {
        using SourceType = std::decay_t<decltype(*srcVec)>;

        // Проверяем, что destination имеет такой же тип
        if (std::holds_alternative<SourceType*>(destination)) {
            auto* destVec = std::get<SourceType*>(destination);

            // Копируем содержимое из `srcVec` в `destVec`
            if (srcVec && destVec) {
                *destVec = *srcVec;
            }
            else {
                //std::cerr << "Ошибка: указатель source или destination равен nullptr." << std::endl;
            }
        }
        else {
            //std::cerr << "Ошибка: типы source и destination не совпадают." << std::endl;
        }
        }, source);
}
template<typename T>
void insertADV(AudioDataVariant& dest, const AudioDataVariant& src) {
    if (auto* destVecFloat = std::get_if<std::vector<T>*>(&dest)) {
        if (auto* srcVecFloat = std::get_if<std::vector<T>*>(&src)) {
            (*destVecFloat)->insert((*destVecFloat)->end(), (*srcVecFloat)->begin(), (*srcVecFloat)->end());
            return;
        }
    }
}


struct InsertVisitor {
    // Для значений типа float
    void operator()(std::vector<float>& dest, const std::vector<float>& src) const {
        dest.insert(dest.end(), src.begin(), src.end());
    }

    // Для значений типа uint8_t
    void operator()(std::vector<uint8_t>& dest, const std::vector<uint8_t>& src) const {
        dest.insert(dest.end(), src.begin(), src.end());
    }

    // Для значений типа int16_t
    void operator()(std::vector<int16_t>& dest, const std::vector<int16_t>& src) const {
        dest.insert(dest.end(), src.begin(), src.end());
    }

    // Для значений типа int32_t
    void operator()(std::vector<int32_t>& dest, const std::vector<int32_t>& src) const {
        dest.insert(dest.end(), src.begin(), src.end());
    }
};

void insertAudioDataVariant(AudioDataVariant& dest, const AudioDataVariant& src) {
    // Используем std::visit дважды для обработки значений вектора
    std::visit([&](auto&& destVec) {
        std::visit([&](auto&& srcVec) {
            // Получаем типы значений внутри векторов
            if (destVec != NULL) {
                using TDest = typename std::decay_t<decltype(*destVec)>::value_type;
                using TSrc = typename std::decay_t<decltype(*srcVec)>::value_type;

                // Проверяем, совпадают ли типы значений
                if (std::is_same_v<TDest, TSrc>) {
                    (*destVec).insert((*destVec).end(), (*srcVec).begin(), (*srcVec).end());
                }
            }
            else {

            }
            }, src);
        }, dest);
}
/*void insertAudioDataVariant(AudioDataVariant& dest, const AudioDataVariant& src, uint8_t currentType) {
    switch (currentType) {
    case _UINT8:
        insertADV<uint8_t>(dest, src);
        break;
    case _INT16:
        insertADV<int16_t>(dest, src);
        break;
    case _INT24:
    case _INT32:
        insertADV<int32_t>(dest, src);
        break;
    case _FLOAT:
        insertADV<float>(dest, src);
        break;
    }
}*/
bool waveformRendererIsNull(WaveformRendererVariant& w);
// Шаблонная функция для инициализации `audioData` нужного типа
template<typename T>
void initializeAudioData(AudioDataVariant& audioData, const std::vector<char>& data) {
    auto* newData = new std::vector<T>(data.size() / sizeof(T));
    std::memcpy(newData->data(), data.data(), data.size());
    audioData = newData;
}
// Функция для инициализации `audioData` в зависимости от `currentType`
void setAudioDataByType(AudioDataVariant& audioData, const std::vector<char>& data, int currentType) {
    switch (currentType) {
    case _FLOAT:
        initializeAudioData<float>(audioData, data);
        break;
    case _UINT8:
        initializeAudioData<uint8_t>(audioData, data);
        break;
    case _INT16:
        initializeAudioData<int16_t>(audioData, data);
        break;
    case _INT24: {
        int j = 0;
        auto* newData = new std::vector<int32_t>(data.size() / 3);
        for (int i = 0; i + 3 < data.size(); i += 3) {
            int32_t value = 0;
            std::memcpy(&value, &data[i], 3);
            bool isNeg = value >> 23;
            if (isNeg)
                value |= 0xFF000000;
            (*newData)[j++] = value;
        }

        audioData = newData;
        break;
    }
    case _INT32:
        initializeAudioData<int32_t>(audioData, data);
        break;
    default:
        break;// throw std::invalid_argument("Unsupported data type");
    }
}
template<typename T>
std::vector<T> _resample(std::vector<T>* inputSamples, float speedFactor, int64_t min, int64_t max) {
    size_t inputSize = inputSamples->size();
    size_t outputSize = static_cast<size_t>(std::round(inputSize / speedFactor));

    std::vector<T> outputSamples;
    outputSamples.reserve(outputSize);

    for (size_t i = 0; i < outputSize; ++i) {
        float originalPosition = i * speedFactor;
        size_t index1 = static_cast<size_t>(originalPosition);
        size_t index2 = index1 + 1;

        if (index2 < inputSize) {
            float fraction = originalPosition - index1;
            T n1 = inputSamples->at(index1) * (1.0f - fraction);
            T n2 = inputSamples->at(index2) * fraction;
            T sum = 0;
            if (n1 + n2 > (T)max)
                sum = (T)max;
            else {
                if (n1 + n2 < (T)min)
                    sum = (T)min;
                else {
                    sum = n1 + n2;
                }
            }
            outputSamples.push_back(sum);
        }
        else {
            if (index1 < inputSize)
                outputSamples.push_back(inputSamples->at(index1));
        }
    }
    return outputSamples;
}
std::vector<char> resampleAudioData(AudioDataVariant& audioData, float speedFactor, uint8_t type) {
    std::vector<char> dat;
    std::visit([&](auto&& data) {
        using DataType = typename std::decay_t<decltype(*data)>::value_type;
        switch (type) {
        case _FLOAT:
            *data = _resample<DataType>(data, speedFactor, -1, 1);
            break;
        case _UINT8:
            *data = _resample<DataType>(data, speedFactor, 0, 255);
            break;
        case _INT16:
            *data = _resample<DataType>(data, speedFactor, SHRT_MIN, SHRT_MAX);
            break;
        case _INT24: {
            *data = _resample<DataType>(data, speedFactor, -8388608, 8388607);
            break;
        }
        case _INT32:
            *data = _resample<DataType>(data, speedFactor, INT_MIN, INT_MAX);
            break;
        default:
            break;
        }
        if (type != _INT24) {
            dat.reserve(data->size() * sizeof(DataType));
            dat.resize(data->size() * sizeof(DataType));
            std::memcpy(dat.data(), (*data).data(), (*data).size() * sizeof(DataType));
        }
        else {
            dat.reserve(data->size() * 3);
            for (int i = 0; i < (*data).size(); i++) {
                int32_t sample = (int32_t)(*data).at(i);
                if (sample < 0) {
                    sample |= 0xFF000000;
                }
                dat.push_back(sample & 0xFF);
                dat.push_back( (sample >> 8 ) & 0xFF);
                dat.push_back( (sample >> 16 ) & 0xFF);
            }
        }
        }, audioData);
    return dat;
}

// Шаблонная функция для создания WaveformRenderer и AudioEditor
template<typename T>
void createRendererAndEditor(
    int typeSize,
    int typeSizeOrig,
    int index,
    uint8_t sampType,
    float scale,
    std::vector<T>* audioDataVec,
    std::vector<char>* audioDataVecCh,
    std::vector<char>* audioDataVecChOrig,
    int height,
    int width,
    std::vector<std::vector<POINT>>& points,
    std::vector<AudioEditorVariant>& audio_editor,
    std::vector<AudioEditor<char>*>& audio_editorCh,
    std::vector<AudioEditor<char>*>& audio_editorChOrig,
    WaveformRendererVariant& w)
{
    if (waveformRendererIsNull(w)) {
        auto* wv = new WaveformRenderer<T>(scale, *audioDataVec, height / 2, width, height, 0);
        wv->SampType = sampType;
        w = wv;
    }
    //points.at(0) = wv->getPoints(0, scale);
    audio_editor.at(index) = new AudioEditor<T>(*audioDataVec);
    audio_editorCh.at(index) = new AudioEditor<char>(*audioDataVecCh);
    audio_editorCh.at(index)->typeSize = typeSize;
    audio_editorChOrig.at(index) = new AudioEditor<char>(*audioDataVecChOrig);
    audio_editorChOrig.at(index)->typeSize = typeSizeOrig;
}

// Функция для создания WaveformRenderer и AudioEditor в зависимости от типа данных
void createWRandAEd(
    int typeSize,
    int typeSizeOrig,
    uint8_t sampType,
    float scale,
    std::vector<AudioDataVariant>& audioData,
    std::vector<std::vector<char>*>& audioDataCh,
    std::vector<std::vector<char>*>& audioDataChOrig,
    int height,
    int width,
    std::vector<std::vector<POINT>>& points,
    std::vector<AudioEditorVariant>& audio_editor,
    std::vector<AudioEditor<char>*>& audio_editorCh,
    std::vector<AudioEditor<char>*>& audio_editorChOrig,
    WaveformRendererVariant& w)
{
    for (int i = 0; i < audioData.size(); i++) {
        std::visit([&](auto&& data) {
            //using DataType = std::decay_t<decltype(*data)>; // Получаем тип вектора из указателя
            createRendererAndEditor(typeSize, typeSizeOrig, i, sampType, scale, data, audioDataCh.at(i), audioDataChOrig.at(i), height, width, points, audio_editor, audio_editorCh, audio_editorChOrig, w);
            }, audioData.at(i));
    }
}
void audio_editorClear(std::vector<AudioEditorVariant>& audio_editor) {
    for (int i = 0; i < audio_editor.size(); i++) {
        std::visit([&](auto&& data) {
            if (data)
                delete data;
            }, audio_editor.at(i));
    }
    audio_editor.clear();
}
void waveformRendererClear(WaveformRendererVariant& waveformRenderer) {
    std::visit([&](auto&& data) {
        if (data)
            delete data;
        }, waveformRenderer);
}
void audioChannelsClear(std::vector<AudioDataVariant>& audioData) {
    for (int i = 0; i < audioData.size(); i++) {
        std::visit([&](auto&& data) {
            if (data)
                delete data;
        }, audioData.at(i));
    }
    audioData.clear();
}
// Функция для получения размера данных в `audioData`
int getAudioDataSize(const AudioDataVariant& audioData) {
    return std::visit([](auto&& data) -> int {
        return data->size();
        }, audioData);
}


/*template<typename T>
void _setStartAndEndPosition(WaveformRenderer<T>* w, float scale, Selection& selection, AudioPlayer* audio_player) {
    int start = static_cast<int>(w->get_offset() + scale * selection.get_left()) * sizeof(T);
    int end;
    if (selection.get_right() == selection.get_left())
        end = INT_MAX;
    else
        end = start + static_cast<int>(scale * (selection.get_right() - selection.get_left())) * sizeof(T);

    audio_player->set_start_position(start);
    audio_player->set_end_position(end);
}*/
void setStartAndEndPosition(int channelsNum, int type, float scale, Selection& selection, AudioPlayer* audio_player, WaveformRendererVariant& w) {
    std::visit([&](auto&& data) {
        if (data != NULL && audio_player != NULL) {
            using dataType = typename std::decay_t<decltype(*data)>::DataType;
            int size = 0;
            if (type != _INT24)
                size = sizeof(dataType);
            else
                size = 3;
            //size *= channelsNum;
            int start = static_cast<int>(data->get_offset() * size * channelsNum + scale * channelsNum * selection.get_left() * size);
            int end;
            if (selection.get_right() == selection.get_left())
                end = INT_MAX;
            else
                end = start + static_cast<int>(scale * channelsNum * size * (selection.get_right() - selection.get_left()));
            int diff = start % size;
            if (diff != 0)
                start += diff;
            diff = end % size;
            if (diff != 0 && end != INT_MAX)
                end += diff;
            audio_player->set_start_position(start);
            audio_player->set_end_position(end);
        }
        //_setStartAndEndPosition(data, scale, selection, audio_player);
        }, w);
}



/*template<typename T>
void updateOffset(WaveformRenderer<T>* wvfrdr, float scale, const RECT& rect, bool isLeftOffset) {
    float offsetChange = 0.05f * scale * (rect.right - rect.left);
    if (isLeftOffset)
        offsetChange *= -1;
    wvfrdr->set_offset(wvfrdr->get_offset() + offsetChange);
}*/
void processOffsetUpdate(float scale, const RECT& rect, WaveformRendererVariant& w, bool isLeftOffset) {
    std::visit([&](auto&& data) {
        //using DataType = std::decay_t<decltype(*data)>;
        float offsetChange = 0.05f * scale * (rect.right - rect.left);
        if (isLeftOffset)
            offsetChange *= -1;
        data->set_offset(data->get_offset() + offsetChange);
        //updateOffset(data, scale, rect, isLeftOffset);
        }, w);
}




/*template<typename T>
void copyAudioPart(AudioEditor<T>* audioEditor, WaveformRenderer<T>* w, float scale, float offset, float left, float right) {
    audioEditor->copy_part_of_Audio(scale, offset, left, right);
}*/
void __copy_part_of_Audio(int type, Selection* selection, AudioEditor<char>* audio_editor, WaveformRendererVariant& w) {
        std::visit([&](auto&& waveformData) {
            using dataType = std::decay_t<decltype(*waveformData)>;
            int size = 0;
            if (type != _INT24)
                size = sizeof(dataType);
            else
                size = 3;
            if (audio_editor)
                audio_editor->copy_part_of_Audio(waveformData->get_scale(), waveformData->get_offset() * size, selection->get_left() * size, selection->get_right() * size);
            }, w);
}

void _copy_part_of_Audio(Selection* selection, AudioEditorVariant& audio_editor, WaveformRendererVariant& w) {
    std::visit([&](auto&& editorData) {
        //using EditorType = std::decay_t<decltype(*editorData)>;
        std::visit([&](auto&& waveformData) {
            //using DataType = std::decay_t<decltype(*waveformData)>;
            if (editorData)
                editorData->copy_part_of_Audio(waveformData->get_scale(), waveformData->get_offset(), selection->get_left(), selection->get_right());
            }, w);
        }, audio_editor);
}

/*void processCancelUndo(AudioDataVariant& audioData, AudioEditorVariant& audio_editor, bool isCancel) {
    std::visit([&](auto&& editorPtr) {
        using editorType = typename std::decay_t<decltype(*editorPtr)>::DataType;
        std::visit([&](auto&& data) {
            using audioDataType = std::decay_t<decltype(*data)>;
            if constexpr (std::is_same_v<audioDataType, std::vector<editorType>>) {
                if (data && editorPtr) {
                    if (isCancel)
                        *data = editorPtr->cancel();
                    else
                        *data = editorPtr->undo();
                }
            }
            }, audioData);
        }, audio_editor);
}*/
/*template<typename T>
void _processEditorAction(WaveformRendererVariant& w, AudioEditor<T>* editorPtr, AudioDataVariant& audioData, Selection* selection, int width, int choice) {
    switch (choice) {
    case _insert:
        *data = editorPtr->insert_to_Audio(waveformData->get_scale(), waveformData->get_offset(), selection->get_left());
        break;
    case _cancel:
        *data = editorPtr->cancel();
        break;
    case _undo:
        *data = editorPtr->undo();
        break;
    case _copy:
        *data = editorPtr->copy_part_of_Audio(waveformData->get_scale(), waveformData->get_offset(), selection->get_left(), selection->get_right());
        break;
    case _cut:
        *data = editorPtr->cut_part_of_Audio(waveformData->get_scale(), waveformData->get_offset(), selection->get_left(), selection->get_right());
        break;
    case _delete_part:
        *data = editorPtr->delete_part_of_Audio(waveformData->get_scale(), waveformData->get_offset(), selection->get_left(), selection->get_right());
        break;
    case _delete_all_except_of_part:
        *data = editorPtr->delete_all_out_of_selection(waveformData->get_scale(), waveformData->get_offset(), selection->get_left(), selection->get_right(), width);
        break;
    default:
        break;
    }
}*/
void processEditorAction(uint8_t sampType, Selection* selection, AudioEditor<char>* audio_editor,
    WaveformRendererVariant& w, std::vector<char>*& audioData, int width, int choice) {
    float scale = 0.0;
    double offset = 0.0;
    std::visit([&](auto&& waveformData) {
        if (waveformData) {
            scale = waveformData->get_scale();
            offset = waveformData->get_offset();
        }
        }, w);
    uint32_t left = selection->get_left();
    uint32_t right = selection->get_right();
    int size = 0;
    switch (sampType) {
    case _INT32:
        size = sizeof(int32_t);
        break;
    case _INT24:
        size = 3;
        break;
    case _INT16:
        size = sizeof(int16_t);
        break;
    case _UINT8:
        size = sizeof(uint8_t);
        break;
    case _FLOAT:
        size = sizeof(float);
        break;
    default:
        break;
    }
    uint32_t leftCh = left * size;
    uint32_t rightCh = right * size;
    uint32_t widthCh = width * size;
    double offsetCh = offset * size;
    float scaleCh = scale;// * size;
    std::vector<char> v;
    if (audio_editor) {
        switch (choice) {
        case _insert:
            v = *audio_editor->insert_to_Audio(scaleCh, offsetCh, leftCh);
            break;
        case _cancel:
            v = *audio_editor->cancel();
            break;
        case _undo:
            v = *audio_editor->undo();
            break;
        case _copy:
            audio_editor->copy_part_of_Audio(scaleCh, offsetCh, leftCh, rightCh);
            break;
        case _cut:
            v = *audio_editor->cut_part_of_Audio(scaleCh, offsetCh, leftCh, rightCh);
            break;
        case _delete_part:
            v = *audio_editor->delete_part_of_Audio(scaleCh, offsetCh, leftCh, rightCh);
            break;
        case _delete_all_except_of_part:
            v = *audio_editor->delete_all_out_of_selection(scaleCh, offsetCh, leftCh, rightCh, widthCh);
            break;
        default:
            break;
        }
    }
    *audioData = v;
}
void processEditorAction(Selection* selection, AudioEditorVariant& audio_editor, 
    WaveformRendererVariant& w, AudioDataVariant& audioData, int width, int choice) {
 
    float scale = 0.0;
    double offset = 0.0;
    std::visit([&](auto&& waveformData) {
        if (waveformData) {
            scale = waveformData->get_scale();
            offset = waveformData->get_offset();
        }
        }, w);
    uint32_t left = selection->get_left();
    uint32_t right = selection->get_right();
    AudioDataVariant data;
    std::visit([&](auto&& editorPtr) {
            if (editorPtr) {
                switch (choice) {
                case _insert:
                    data = editorPtr->insert_to_Audio(scale, offset, left);
                    break;
                case _cancel:
                    data = editorPtr->cancel();
                    break;
                case _undo:
                    data = editorPtr->undo();
                    break;
                case _copy:
                    editorPtr->copy_part_of_Audio(scale, offset, left, right);
                    break;
                case _cut:
                    data = editorPtr->cut_part_of_Audio(scale, offset, left, right);
                    break;
                case _delete_part:
                    data = editorPtr->delete_part_of_Audio(scale, offset, left, right);
                    break;
                case _delete_all_except_of_part:
                    data = editorPtr->delete_all_out_of_selection(scale, offset, left, right, width);
                    break;
                default:
                    break;
                }
            }
        }, audio_editor);
    copyAudioData(data, audioData);
}


void processWaveformRendering(
    int currChannel,
    std::vector<AudioDataVariant>& audioData,
    WaveformRendererVariant& w,
    std::vector<AudioEditorVariant>& audio_editor,
    std::vector<std::vector<POINT>>& points,
    int index,
    int height,
    int width,
    int y_position,
    POINT cursorPos,
    float scale, int start_x_pos
) {
    for (int i = currChannel; i < currChannel + 2 && i < points.size(); i++) {
        std::visit([&](auto&& dataPtr) {
            using DataType = typename std::decay_t<decltype(*dataPtr)>::value_type;

            auto* waveformRenderer = std::get<WaveformRenderer<DataType>*>(w);
            auto* audioEditor = std::get<AudioEditor<DataType>*>(audio_editor.at(i));

            if (waveformRenderer && audioEditor && dataPtr) {
                waveformRenderer->set_audioData(*dataPtr);
                waveformRenderer->set_height(height);
                waveformRenderer->set_width(width);
                waveformRenderer->set_y_position(y_position + i * height);

                points.at(index + i) = waveformRenderer->getPoints(cursorPos.x, scale, start_x_pos);

                audioEditor->SetData(*dataPtr);
            }
        }, audioData.at(index + i));
    }
}
void waveformRendererSetNull(WaveformRendererVariant& w) {

    std::visit([&](auto&& dataPtr) {
        (dataPtr = NULL);
    }, w);
}
bool waveformRendererIsNull(WaveformRendererVariant& w) {
    
    return std::visit([&](auto&& dataPtr) -> bool {
        return (dataPtr == NULL);
        }, w);
}

void DrawW(int currFirstChannel, std::vector<std::vector<POINT>>& points, HDC hdc, 
    int width, int height, int y_pos, int start_x_pos, Buffer* buffer) {
    for (int i = currFirstChannel; i < currFirstChannel + 2 && i < points.size(); i++) {
        int inc = 0;
        std::vector<POINT> p = points.at(i);
        if (currFirstChannel != 0) {
            for (int j = 0; j < p.size(); j++) {
                p.at(j).y -= height * currFirstChannel;
            }
        }
        if ((i != currFirstChannel + 1) && (points.size() > 1))
            inc = -(height * (i + 1));
        else
            inc = -(height * currFirstChannel);
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(66, 106, 216));

        SelectObject(buffer->hdc, hPen);
        Polyline(buffer->hdc, p.data(), p.size());

        DeleteObject(hPen);

        hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
        SelectObject(buffer->hdc, hPen);
        MoveToEx(buffer->hdc, start_x_pos, y_pos + inc, NULL);
        LineTo(buffer->hdc, width + start_x_pos, y_pos + inc);

        DeleteObject(hPen);

        DeleteObject(hBrush);
    }
}
void DrawL(int currFirstChannel, std::vector<std::vector<POINT>>& points, HDC hdc,
    int width, int height, int y_pos, int start_x_pos, Buffer* buffer) {
    for (int i = currFirstChannel; i < currFirstChannel + 2 && i < points.size(); i++) {
        int inc = 0;
        std::vector<POINT> p = points.at(i);
        if (currFirstChannel != 0) {
            for (int j = 0; j < p.size(); j++) {
                p.at(j).y -= height * currFirstChannel;
            }
        }
        if ((i != currFirstChannel + 1) && (points.size() > 1))
            inc = -(height * (i + 1));
        else
            inc = -(height * currFirstChannel);
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(66, 106, 216));
        SelectObject(buffer->hdc, hPen);
        hBrush = CreateSolidBrush(RGB(128, 0, 128));
        for (const auto& point : p) {
            MoveToEx(buffer->hdc, point.x, point.y, NULL);
            LineTo(buffer->hdc, point.x, y_pos + inc);

            Ellipse(buffer->hdc, point.x - 2, point.y - 2, point.x + 2, point.y + 2);
        }
        DeleteObject(hBrush);
        hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
        SelectObject(buffer->hdc, hPen);

        MoveToEx(buffer->hdc, start_x_pos, y_pos + inc, NULL);
        LineTo(buffer->hdc, width + start_x_pos, y_pos + inc);
        DeleteObject(hPen);
    }
}

void processWaveformDrawing(
    int currFirstChannel,
    WaveformRendererVariant & w,
    std::vector<std::vector<POINT>>& points,
    HDC hdc,
    int start_x_pos,
    Buffer* screen_buffer
) 
{
    std::visit([&](auto&& waveformRendererPtr) {
        //using DataType = typename std::decay_t<decltype(*waveformRendererPtr)>;
        if (waveformRendererPtr) {
            if (!waveformRendererPtr->get_isLower()) {
                DrawW(
                    currFirstChannel,
                    points,
                    hdc,
                    waveformRendererPtr->get_width(),
                    waveformRendererPtr->get_height(),
                    waveformRendererPtr->get_y_position(),
                    start_x_pos,
                    screen_buffer
                );
            }
            else {
                DrawL(
                    currFirstChannel,
                    points,
                    hdc,
                    waveformRendererPtr->get_width(),
                    waveformRendererPtr->get_height(),
                    waveformRendererPtr->get_y_position(),
                    start_x_pos,
                    screen_buffer
                );
            }
        }
        }, w);
}

void processScale(WaveformRendererVariant& w, float& scale) {
    std::visit([&](auto&& waveformRendererPtr) {
        //using DataType = typename std::decay_t<decltype(*waveformRendererPtr)>;
        if (waveformRendererPtr) {
            scale = waveformRendererPtr->get_scale();
        }
        }, w);
}

std::pair<vector<int16_t>, int> resample(const std::vector<int16_t>& inputSamples, int originalSampleRate, int targetSampleRate) {
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
}
