#pragma once
#include <vector>
//#include <random>
//#include <algorithm>
//#include <cstdint>
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
class AudioEncodingConverter
{
public:
    std::vector<std::vector<char>*>& changeEncoding(uint8_t targetType, std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& get_basicData();
    uint8_t get_currentType();
    uint8_t get_typeSize();
    uint8_t get_origTypeSize();
    void set_currentType(uint8_t currType);
    uint8_t get_origType();
    void set_origType(uint8_t origType);
    void set_origTypeSize(uint8_t origTypeSize);
    void set_typeSize(uint8_t typeSize);
    void set_basicData(std::vector<std::vector<char>*>& basicData);
private:
    template <typename InputType, typename OutputType>
    std::vector<std::vector<char>*>& convertAudioData(uint8_t inTypeSize, uint8_t outTypeSize, std::vector<std::vector<char>*>& audioData, float scale, float offset, uint8_t conversionId);
    // Преобразования из unsigned 8-bit в другие форматы
    std::vector<std::vector<char>*>& unsigned8BitToSigned16Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& unsigned8BitToSigned24Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& unsigned8BitToSigned32Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& unsigned8BitToFloat(std::vector<std::vector<char>*>& audioData);

    // Преобразования из signed 16-bit в другие форматы
    std::vector<std::vector<char>*>& signed16BitToUnsigned8Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& signed16BitToSigned24Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& signed16BitToSigned32Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& signed16BitToFloat(std::vector<std::vector<char>*>& audioData);

    // Преобразования из signed 24-bit в другие форматы
    std::vector<std::vector<char>*>& signed24BitToUnsigned8Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& signed24BitToSigned16Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& signed24BitToSigned32Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& signed24BitToFloat(std::vector<std::vector<char>*>& audioData);

    // Преобразования из signed 32-bit в другие форматы
    std::vector<std::vector<char>*>& signed32BitToUnsigned8Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& signed32BitToSigned16Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& signed32BitToSigned24Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& signed32BitToFloat(std::vector<std::vector<char>*>& audioData);

    // Преобразования из IEEE float в другие форматы
    std::vector<std::vector<char>*>& floatToUnsigned8Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& floatToSigned16Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& floatToSigned24Bit(std::vector<std::vector<char>*>& audioData);
    std::vector<std::vector<char>*>& floatToSigned32Bit(std::vector<std::vector<char>*>& audioData);

    std::vector<std::vector<char>*> basicData;
    //std::vector<std::vector<char>*> basicDataCopy;
    uint8_t currentType = 0;
    uint8_t typeSize = 0;
    uint8_t origType = 0;
    uint8_t origTypeSize = 0;
};

