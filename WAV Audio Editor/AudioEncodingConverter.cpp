#include "AudioEncodingConverter.h"

template <typename InputType, typename OutputType>
std::vector<std::vector<char>*>& AudioEncodingConverter::convertAudioData(uint8_t inTypeSize, uint8_t outTypeSize, std::vector<std::vector<char>*>& audioData, float scale, float offset, uint8_t conversionId) {
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char>* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += inTypeSize) {
            InputType sample = 0;
            memcpy(&sample, reinterpret_cast<const void*>(channel->data() + j), inTypeSize);
            OutputType convertedSample = 0;
            switch (conversionId) {
            case 0: {
                convertedSample = (OutputType)(sample * scale + offset);
                break;
            }
            case 1: {
                convertedSample = (OutputType)((double)(sample + offset) * scale);
                break;
            }
            case 2: {
                if constexpr (std::is_floating_point<decltype(sample)>::value) {
                    //throw std::invalid_argument("Sample type cannot be a floating point!"s);
                }
                else {
                    bool isNeg = sample >> 23;
                    if (isNeg) {
                        sample |= 0xFF000000;
                    }
                    convertedSample = (OutputType)((double)(sample + offset) * scale);
                }
                break;
            }
            default:
                break;
            }
            char* ptr = reinterpret_cast<char*>(&convertedSample);
            convertedData.insert(convertedData.end(), ptr, ptr + outTypeSize);
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::changeEncoding(uint8_t targetType, std::vector<std::vector<char>*>& audioData)
{
    if (origType == targetType) {
        /*void* ptr1 = &basicData;
        void* ptr2 = &audioData;
        uint32_t* p1 = (uint32_t*)ptr1;
        uint32_t* p2 = (uint32_t*)ptr2;*/
        if (&basicData == &audioData) {
            return audioData;
        }
        for (int i = 0; i < audioData.size(); i++) {
            if (audioData.at(i)) {
                delete audioData.at(i);
                audioData.at(i) = NULL;
            }
        }
        audioData.clear();
        for (auto& srcVec : basicData) {
            audioData.push_back(new std::vector<char>(*srcVec));
        }
        return audioData;
    }
    int conversionId = origType * 10 + targetType;
    switch (conversionId) {
        // UNSIGNED 8-bit → другие форматы
    case 1: return convertAudioData<uint8_t, int16_t>(sizeof(uint8_t), sizeof(int16_t), audioData, 257, -32768.0000000000000f, 0);//unsigned8BitToSigned16Bit(audioData);
    case 2: return convertAudioData<uint8_t, int32_t>(sizeof(uint8_t), 3, audioData, 65793, -8388608, 0);//unsigned8BitToSigned24Bit(audioData);
    case 3: return convertAudioData<uint8_t, int32_t>(sizeof(uint8_t), sizeof(int32_t), audioData, 16843009, -2147483648, 0);//unsigned8BitToSigned32Bit(audioData);
    case 4: return convertAudioData<uint8_t, float>(sizeof(uint8_t), sizeof(float), audioData, 1 / 128.00000000f, -128, 1);//unsigned8BitToFloat(audioData);
   
        // SIGNED 16-bit → другие форматы
    case 10: return convertAudioData<int16_t, uint8_t>(sizeof(int16_t), sizeof(uint8_t), audioData, 1 / 257.00000000f, 32768, 1);//signed16BitToUnsigned8Bit(audioData);
    case 12: return convertAudioData<int16_t, int32_t>(sizeof(int16_t), 3, audioData, 256, 0, 0);//signed16BitToSigned24Bit(audioData);
    case 13: return convertAudioData<int16_t, int32_t>(sizeof(int16_t), sizeof(int32_t), audioData, 65536, 0, 0);//signed16BitToSigned32Bit(audioData);
    case 14: return convertAudioData<int16_t, float>(sizeof(int16_t), sizeof(float), audioData, 1 / 32768.00000000f, 0, 0);//signed16BitToFloat(audioData);

        // SIGNED 24-bit → другие форматы
    case 20: return convertAudioData<int32_t, uint8_t>(3, sizeof(uint8_t), audioData, 1 / 65793.00000000f, 8388608, 2);//signed24BitToUnsigned8Bit(audioData);
    case 21: return convertAudioData<int32_t, int16_t>(3, sizeof(int16_t), audioData, 1 / 256.00000000f, 0, 2);//signed24BitToSigned16Bit(audioData);
    case 23: return convertAudioData<int32_t, int32_t>(3, sizeof(int32_t), audioData, 256, 0, 2);//signed24BitToSigned32Bit(audioData);
    case 24: return convertAudioData<int32_t, float>(3, sizeof(float), audioData, 1 / 8388608.00000000f, 0, 2);//signed24BitToFloat(audioData);

        // SIGNED 32-bit → другие форматы
    case 30: return convertAudioData<int32_t, uint8_t>(sizeof(int32_t), sizeof(uint8_t), audioData, 1 / 16843009.00000000f, 2147483648, 1);//signed32BitToUnsigned8Bit(audioData);
    case 31: return convertAudioData<int32_t, int16_t>(sizeof(int32_t), sizeof(int16_t), audioData, 1 / 65536.00000000f, 0, 0);//signed32BitToSigned16Bit(audioData);
    case 32: return convertAudioData<int32_t, int32_t>(sizeof(int32_t), 3, audioData, 1 / 256.00000000f, 0, 0);//signed32BitToSigned24Bit(audioData);
    case 34: return convertAudioData<int32_t, float>(sizeof(int32_t), sizeof(float), audioData, 1 / 2147483648.00000000f, 0, 0);//signed32BitToFloat(audioData);

        // IEEE FLOAT → другие форматы
    case 40: return convertAudioData<float, uint8_t>(sizeof(float), sizeof(uint8_t), audioData, 128, 128, 0);//floatToUnsigned8Bit(audioData);
    case 41: return convertAudioData<float, int16_t>(sizeof(float), sizeof(int16_t), audioData, 32768, 0, 0);//floatToSigned16Bit(audioData);
    case 42: return convertAudioData<float, int32_t>(sizeof(float), 3, audioData, 8388608, 0, 0);//floatToSigned24Bit(audioData);
    case 43: return convertAudioData<float, int32_t>(sizeof(float), sizeof(int32_t), audioData, 2147483648, 0, 0);//floatToSigned32Bit(audioData);

    default:
        return audioData;
        //throw std::invalid_argument("Unsupported conversion combination");
    }
}

std::vector<std::vector<char>*>& AudioEncodingConverter::get_basicData()
{
    return this->basicData;
}

uint8_t AudioEncodingConverter::get_currentType()
{
    return currentType;
}

uint8_t AudioEncodingConverter::get_typeSize()
{
    return typeSize;
}

uint8_t AudioEncodingConverter::get_origTypeSize()
{
    return origTypeSize;
}

void AudioEncodingConverter::set_currentType(uint8_t currType)
{
    currentType = currType;
}

uint8_t AudioEncodingConverter::get_origType()
{
    return origType;
}

void AudioEncodingConverter::set_origType(uint8_t origType)
{
    this->origType = origType;
}

void AudioEncodingConverter::set_origTypeSize(uint8_t origTypeSize)
{
    this->origTypeSize = origTypeSize;
}

void AudioEncodingConverter::set_typeSize(uint8_t typeSize)
{
    this->typeSize = typeSize;
}

void AudioEncodingConverter::set_basicData(std::vector<std::vector<char>*>& basicData)
{
    this->basicData = basicData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::unsigned8BitToSigned16Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0;  i < basicData.size();i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0;  j < channel->size(); j++) {
            uint8_t sample = static_cast<uint8_t>(channel->at(j));

            int16_t convertedSample = (sample * 257) - 32768.0000000000000f;

            /*if (convertedSample >= 0) {
                if ((int)(convertedSample + int(noiseDist(generator) * 32768.00000000000000000f)) < 32767)
                    convertedSample += static_cast<int16_t>(noiseDist(generator) * 32768.00000000000000000f);
            }
            else {
                if ((int)(convertedSample - int(noiseDist(generator) * 32768.00000000000000000f)) > -32768)
                    convertedSample -= static_cast<int16_t>(noiseDist(generator) * 32768.00000000000000000f);
            }*/
            convertedData.push_back(convertedSample & 0xFF);
            convertedData.push_back((convertedSample >> 8) & 0xFF);
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::unsigned8BitToSigned24Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j++) {
            uint8_t sample = static_cast<uint8_t>(channel->at(j));

            int32_t convertedSample = (sample * 65793) - 8388608;

            /*if (convertedSample >= 0) {
                if ((int)(convertedSample + int(noiseDist(generator) * 8388608.00000000000000000f)) < 8388607)
                    convertedSample += static_cast<int32_t>(noiseDist(generator) * 8388608.00000000000000000f);
            }
            else {
                if ((int)(convertedSample - int(noiseDist(generator) * 8388608.00000000000000000f)) > -8388608)
                    convertedSample -= static_cast<int32_t>(noiseDist(generator) * 8388608.00000000000000000f);
            }*/
            convertedData.push_back(convertedSample & 0xFF);
            convertedData.push_back((convertedSample >> 8) & 0xFF);
            convertedData.push_back((convertedSample >> 16) & 0xFF);
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::unsigned8BitToSigned32Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j++) {
            uint8_t sample = static_cast<uint8_t>(channel->at(j));

            int32_t convertedSample = (sample * 16843009) - 2147483648;

            /*if (convertedSample >= 0) {
                if ((int64_t)(convertedSample + (int64_t)(noiseDist(generator) * 2147483648.00000000000000000f)) < 2147483647)
                    convertedSample += static_cast<int32_t>(noiseDist(generator) * 2147483648.00000000000000000f);
            }
            else {
                if ((int64_t)(convertedSample - (int64_t)(noiseDist(generator) * 2147483648.00000000000000000f)) > -2147483648)
                    convertedSample -= static_cast<int32_t>(noiseDist(generator) * 2147483648.00000000000000000f);
            }*/
            convertedData.push_back(convertedSample & 0xFF);
            convertedData.push_back((convertedSample >> 8) & 0xFF);
            convertedData.push_back((convertedSample >> 16) & 0xFF);
            convertedData.push_back((convertedSample >> 24) & 0xFF);
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::unsigned8BitToFloat(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j++) {
            uint8_t sample = static_cast<uint8_t>(channel->at(j));

            float convertedSample = float((int8_t)(sample - 128)) / 128.00000000f;

            /*if (convertedSample >= 0) {
                if (convertedSample + noiseDist(generator) < 1.000000000f)
                    convertedSample += noiseDist(generator);
            }
            else {
                if (convertedSample - noiseDist(generator) < -1.000000000f)
                    convertedSample -= noiseDist(generator);
            }*/
            float* ptr = &convertedSample;
            convertedData.insert(convertedData.end(), reinterpret_cast<char*>(ptr), reinterpret_cast<char*>(ptr) + sizeof(float));
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::signed16BitToUnsigned8Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += sizeof(int16_t)) {
            int16_t sample = 0;
            memcpy(&sample, channel->data() + j, sizeof(int16_t));
            uint16_t usample = (uint16_t)(sample + 32768);
            uint8_t convertedSample = (uint8_t)(usample / 257);

            /*if (convertedSample > 128) {
                if ((uint8_t)(convertedSample - (uint8_t)(noiseDist(generator) * 256)) > 0)
                    convertedSample -= static_cast<uint8_t>(noiseDist(generator) * 256);
            }
            else {
                if ((int)(convertedSample + int(noiseDist(generator) * 256)) < 255)
                    convertedSample += static_cast<uint8_t>(noiseDist(generator) * 256);
            }*/
            convertedData.push_back((convertedSample));
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::signed16BitToSigned24Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += sizeof(int16_t)) {
            int16_t sample = 0;
            memcpy(&sample, channel->data() + j, sizeof(int16_t));

            int32_t convertedSample = sample * 256;

            /*if (convertedSample >= 0) {
                if ((int)(convertedSample + int(noiseDist(generator) * 8388608.00000000000000000f)) < 8388607)
                    convertedSample += static_cast<int32_t>(noiseDist(generator) * 8388608.00000000000000000f);
            }
            else {
                if ((int)(convertedSample - int(noiseDist(generator) * 8388608.00000000000000000f)) > -8388608)
                    convertedSample -= static_cast<int32_t>(noiseDist(generator) * 8388608.00000000000000000f);
            }*/
            
            convertedData.push_back(convertedSample & 0xFF);
            convertedData.push_back((convertedSample >> 8) & 0xFF);
            convertedData.push_back((convertedSample >> 16) & 0xFF);
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::signed16BitToSigned32Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += sizeof(int16_t)) {
            int16_t sample = 0;
            memcpy(&sample, channel->data() + j, sizeof(int16_t));

            int32_t convertedSample = sample * 256 * 256;

            /*if (convertedSample >= 0) {
                if ((int64_t)(convertedSample + (int64_t)(noiseDist(generator) * 2147483648.00000000000000000f)) < 2147483647)
                    convertedSample += static_cast<int32_t>(noiseDist(generator) * 2147483648.00000000000000000f);
            }
            else {
                if ((int64_t)(convertedSample - (int64_t)(noiseDist(generator) * 2147483648.00000000000000000f)) > -2147483648)
                    convertedSample -= static_cast<int32_t>(noiseDist(generator) * 2147483648.00000000000000000f);
            }*/
            convertedData.push_back(convertedSample & 0xFF);
            convertedData.push_back((convertedSample >> 8) & 0xFF);
            convertedData.push_back((convertedSample >> 16) & 0xFF);
            convertedData.push_back((convertedSample >> 24) & 0xFF);
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::signed16BitToFloat(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += sizeof(int16_t)) {
            int16_t sample = 0;
            memcpy(&sample, channel->data() + j, sizeof(int16_t));

            float convertedSample = float(sample / 32768.00000000f);

            /*if (convertedSample >= 0) {
                if (convertedSample + noiseDist(generator) < 1.000000000f)
                    convertedSample += noiseDist(generator);
            }
            else {
                if (convertedSample - noiseDist(generator) < -1.000000000f)
                    convertedSample -= noiseDist(generator);
            }*/
            float* ptr = &convertedSample;
            convertedData.insert(convertedData.end(), reinterpret_cast<char*>(ptr), reinterpret_cast<char*>(ptr) + sizeof(float));
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::signed24BitToUnsigned8Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += 3) {
            int32_t sample = 0;
            memcpy(&sample, channel->data() + j, 3);
            bool isNeg = sample >> 23;
            if (isNeg)
                sample |= 0xFF000000;
            uint32_t usample = (uint32_t)(sample + 8388608);
            uint8_t convertedSample = (uint8_t)(usample / 65793);

            /*if (convertedSample > 128) {
                if ((uint8_t)(convertedSample - (uint8_t)(noiseDist(generator) * 256)) > 0)
                    convertedSample -= static_cast<uint8_t>(noiseDist(generator) * 256);
            }
            else {
                if ((int)(convertedSample + int(noiseDist(generator) * 256)) < 255)
                    convertedSample += static_cast<uint8_t>(noiseDist(generator) * 256);
            }*/
            convertedData.push_back((convertedSample));
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::signed24BitToSigned16Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += 3) {
            int32_t sample = 0;
            memcpy(&sample, channel->data() + j, 3);
            bool isNeg = sample >> 23;
            if (isNeg)
                sample |= 0xFF000000;
            int16_t convertedSample = sample / 256;

            /*if (convertedSample < 0) {
                if ((int32_t)(convertedSample + (int32_t)(noiseDist(generator) * 32768.00000000000000000f)) < 32767)
                    convertedSample += static_cast<int16_t>(noiseDist(generator) * 32768.00000000000000000f);
            }
            else {
                if ((int32_t)(convertedSample - (int32_t)(noiseDist(generator) * 32768.00000000000000000f)) > -32768)
                    convertedSample -= static_cast<int16_t>(noiseDist(generator) * 32768.00000000000000000f);
            }*/
            convertedData.push_back(convertedSample & 0xFF);
            convertedData.push_back((convertedSample >> 8) & 0xFF);
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::signed24BitToSigned32Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += 3) {
            int32_t sample = 0;
            memcpy(&sample, channel->data() + j, 3);
            bool isNeg = sample >> 23;
            if (isNeg)
                sample |= 0xFF000000;
            int32_t convertedSample = sample * 256;

            /*if (convertedSample >= 0) {
                if ((int64_t)(convertedSample + (int64_t)(noiseDist(generator) * 2147483648.00000000000000000f)) < 2147483647)
                    convertedSample += static_cast<int32_t>(noiseDist(generator) * 2147483648.00000000000000000f);
            }
            else {
                if ((int64_t)(convertedSample - (int64_t)(noiseDist(generator) * 2147483648.00000000000000000f)) > -2147483648)
                    convertedSample -= static_cast<int32_t>(noiseDist(generator) * 2147483648.00000000000000000f);
            }*/
            convertedData.push_back(convertedSample & 0xFF);
            convertedData.push_back((convertedSample >> 8) & 0xFF);
            convertedData.push_back((convertedSample >> 16) & 0xFF);
            convertedData.push_back((convertedSample >> 24) & 0xFF);
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::signed24BitToFloat(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += 3) {
            int32_t sample = 0;
            memcpy(&sample, channel->data() + j, 3);
            bool isNeg = sample >> 23;
            if (isNeg)
                sample |= 0xFF000000;
            float convertedSample = float(sample / 8388608.00000000f);

            /*if (convertedSample >= 0) {
                if (convertedSample + noiseDist(generator) < 1.000000000f)
                    convertedSample += noiseDist(generator);
            }
            else {
                if (convertedSample - noiseDist(generator) < -1.000000000f)
                    convertedSample -= noiseDist(generator);
            }*/
            float* ptr = &convertedSample;
            convertedData.insert(convertedData.end(), reinterpret_cast<char*>(ptr), reinterpret_cast<char*>(ptr) + sizeof(float));
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::signed32BitToUnsigned8Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += sizeof(int32_t)) {
            int32_t sample = 0;
            memcpy(&sample, channel->data() + j, sizeof(int32_t));
            uint32_t usample = (uint32_t)(sample + 2147483648);
            uint8_t convertedSample = (uint8_t)(usample / 16843009);

            /*if (convertedSample > 128) {
                if ((uint8_t)(convertedSample - (uint8_t)(noiseDist(generator) * 256)) > 0)
                    convertedSample -= static_cast<uint8_t>(noiseDist(generator) * 256);
            }
            else {
                if ((int)(convertedSample + int(noiseDist(generator) * 256)) < 255)
                    convertedSample += static_cast<uint8_t>(noiseDist(generator) * 256);
            }*/
            convertedData.push_back((convertedSample));
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::signed32BitToSigned16Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += sizeof(int32_t)) {
            int32_t sample = 0;
            memcpy(&sample, channel->data() + j, sizeof(int32_t));

            int16_t convertedSample = sample / 256 / 256;

            /*if (convertedSample < 0) {
                if ((int32_t)(convertedSample + (int32_t)(noiseDist(generator) * 32768.00000000000000000f)) < 32767)
                    convertedSample += static_cast<int16_t>(noiseDist(generator) * 32768.00000000000000000f);
            }
            else {
                if ((int32_t)(convertedSample - (int32_t)(noiseDist(generator) * 32768.00000000000000000f)) > -32768)
                    convertedSample -= static_cast<int16_t>(noiseDist(generator) * 32768.00000000000000000f);
            }*/
            convertedData.push_back(convertedSample & 0xFF);
            convertedData.push_back((convertedSample >> 8) & 0xFF);
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::signed32BitToSigned24Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += sizeof(int32_t)) {
            int32_t sample = 0;
            memcpy(&sample, channel->data() + j, sizeof(int32_t));

            int32_t convertedSample = sample / 256;

            /*if (convertedSample < 0) {
                if ((int)(convertedSample + int(noiseDist(generator) * 8388608.00000000000000000f)) < 8388607)
                    convertedSample += static_cast<int32_t>(noiseDist(generator) * 8388608.00000000000000000f);
            }
            else {
                if ((int)(convertedSample - int(noiseDist(generator) * 8388608.00000000000000000f)) > -8388608)
                    convertedSample -= static_cast<int32_t>(noiseDist(generator) * 8388608.00000000000000000f);
            }*/
            convertedData.push_back(convertedSample & 0xFF);
            convertedData.push_back((convertedSample >> 8) & 0xFF);
            convertedData.push_back((convertedSample >> 16) & 0xFF);
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::signed32BitToFloat(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += sizeof(int32_t)) {
            int32_t sample = 0;
            memcpy(&sample, channel->data() + j, sizeof(int32_t));

            float convertedSample = float(sample / 2147483648.00000000f);

            /*if (convertedSample >= 0) {
                if (convertedSample + noiseDist(generator) < 1.000000000f)
                    convertedSample += noiseDist(generator);
            }
            else {
                if (convertedSample - noiseDist(generator) < -1.000000000f)
                    convertedSample -= noiseDist(generator);
            }*/
            float* ptr = &convertedSample;
            convertedData.insert(convertedData.end(), reinterpret_cast<char*>(ptr), reinterpret_cast<char*>(ptr) + sizeof(float));
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::floatToUnsigned8Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += sizeof(float)) {
            float sample = 0.0f;
            memcpy(&sample, channel->data() + j, sizeof(float));
            uint8_t convertedSample = (sample * 128) + 128;

            /*if (convertedSample > 128) {
                if ((uint8_t)(convertedSample - (uint8_t)(noiseDist(generator) * 256)) > 0)
                    convertedSample -= static_cast<uint8_t>(noiseDist(generator) * 256);
            }
            else {
                if ((int)(convertedSample + int(noiseDist(generator) * 256)) < 255)
                    convertedSample += static_cast<uint8_t>(noiseDist(generator) * 256);
            }*/
            convertedData.push_back((convertedSample));
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::floatToSigned16Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += sizeof(float)) {
            float sample = 0.0f;
            memcpy(&sample, channel->data() + j, sizeof(float));

            int16_t convertedSample = sample * 32768;

            /*if (convertedSample < 0) {
                if ((int32_t)(convertedSample + (int32_t)(noiseDist(generator) * 32768.00000000000000000f)) < 32767)
                    convertedSample += static_cast<int16_t>(noiseDist(generator) * 32768.00000000000000000f);
            }
            else {
                if ((int32_t)(convertedSample - (int32_t)(noiseDist(generator) * 32768.00000000000000000f)) > -32768)
                    convertedSample -= static_cast<int16_t>(noiseDist(generator) * 32768.00000000000000000f);
            }*/
            convertedData.push_back(convertedSample & 0xFF);
            convertedData.push_back((convertedSample >> 8) & 0xFF);
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::floatToSigned24Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += sizeof(float)) {
            float sample = 0.0f;
            memcpy(&sample, channel->data() + j, sizeof(float));

            int32_t convertedSample = sample * 8388608;

            /*if (convertedSample < 0) {
                if ((int)(convertedSample + int(noiseDist(generator) * 8388608.00000000000000000f)) < 8388607)
                    convertedSample += static_cast<int32_t>(noiseDist(generator) * 8388608.00000000000000000f);
            }
            else {
                if ((int)(convertedSample - int(noiseDist(generator) * 8388608.00000000000000000f)) > -8388608)
                    convertedSample -= static_cast<int32_t>(noiseDist(generator) * 8388608.00000000000000000f);
            }*/
            convertedData.push_back(convertedSample & 0xFF);
            convertedData.push_back((convertedSample >> 8) & 0xFF);
            convertedData.push_back((convertedSample >> 16) & 0xFF);
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}

std::vector<std::vector<char>*>& AudioEncodingConverter::floatToSigned32Bit(std::vector<std::vector<char>*>& audioData)
{
    //std::default_random_engine generator;
    //std::uniform_real_distribution<float> noiseDist(0.000f, 0.010f);
    for (size_t i = 0; i < basicData.size(); i++) {
        std::vector<char> convertedData;
        const std::vector<char> const* channel = basicData.at(i);
        for (size_t j = 0; j < channel->size(); j += sizeof(float)) {
            float sample = 0.0f;
            memcpy(&sample, channel->data() + j, sizeof(float));

            int32_t convertedSample = sample * 2147483648;

            /*if (convertedSample >= 0) {
                if ((int64_t)(convertedSample + (int64_t)(noiseDist(generator) * 2147483648.00000000000000000f)) < 2147483647)
                    convertedSample += static_cast<int32_t>(noiseDist(generator) * 2147483648.00000000000000000f);
            }
            else {
                if ((int64_t)(convertedSample - (int64_t)(noiseDist(generator) * 2147483648.00000000000000000f)) > -2147483648)
                    convertedSample -= static_cast<int32_t>(noiseDist(generator) * 2147483648.00000000000000000f);
            }*/
            convertedData.push_back(convertedSample & 0xFF);
            convertedData.push_back((convertedSample >> 8) & 0xFF);
            convertedData.push_back((convertedSample >> 16) & 0xFF);
            convertedData.push_back((convertedSample >> 24) & 0xFF);
        }
        *audioData.at(i) = std::move(convertedData);
    }
    return audioData;
}
