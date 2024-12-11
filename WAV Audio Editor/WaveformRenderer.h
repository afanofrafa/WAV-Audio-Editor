#pragma once

#include <vector>
#include <windows.h>
#include <algorithm>
#include <limits>

using namespace std;

#define MIN_SHOWN_LENGTH 300
#define MIN_SAMPLES_PER_SCREEN 4
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
template<class SampleType>
class WaveformRenderer {
private:
    double offset;
    float scale = 0.0f;
    vector<SampleType> audioData;
    int x_position = 0;
    int y_position;
    int width;
    int height;
    bool isLower;

public:
    WaveformRenderer() = delete;
    WaveformRenderer(float scale, vector<SampleType>& audioData, int y_position, int width, int height, double offset);

    void set_audioData(vector<SampleType>& audioData);
    vector<SampleType>& get_audioData();

    void set_isLower(bool isLower);
    bool get_isLower();

    void set_x_position(int x_position);
    int get_x_position();

    void set_y_position(int y_position);
    int get_y_position();

    void set_width(int width);
    int get_width();

    void set_height(int height);
    int get_height();

    void set_scale(float scale);
    float get_scale();

    void set_offset(double offset);
    double get_offset();

	//int get_sizeofType();

    vector<POINT> getPoints(int new_x, float new_scale, int start_x_pos);

	uint8_t SampType = -1;

	using DataType = SampleType;
};
template <class SampleType>
WaveformRenderer<SampleType>::WaveformRenderer(float scale, vector<SampleType>& audioData, int y_position, int width, int height, double offset) {
	this->audioData = audioData;
	this->width = width;
	this->height = height;
	this->offset = offset;
	this->y_position = y_position;
	this->scale = scale;
}
/*template <class SampleType>
int WaveformRenderer<SampleType>::get_sizeofType() {
	return sizeof(SampleType);
}*/
template <class SampleType>
void WaveformRenderer<SampleType>::set_audioData(vector<SampleType>& audioData) {
	this->audioData = audioData;
}

template <class SampleType>
vector<SampleType>& WaveformRenderer<SampleType>::get_audioData() {
	return audioData;
}

template <class SampleType>
void WaveformRenderer<SampleType>::set_isLower(bool isLower) {
	this->isLower = isLower;
}

template <class SampleType>
bool WaveformRenderer<SampleType>::get_isLower() {
	return this->isLower;
}

template <class SampleType>
void WaveformRenderer<SampleType>::set_x_position(int x_position) {
	this->x_position = x_position;
}

template <class SampleType>
int WaveformRenderer<SampleType>::get_x_position() {
	return this->x_position;
}

template <class SampleType>
void WaveformRenderer<SampleType>::set_y_position(int y_position) {
	this->y_position = y_position;
}

template <class SampleType>
int WaveformRenderer<SampleType>::get_y_position() {
	return this->y_position;
}

template <class SampleType>
void WaveformRenderer<SampleType>::set_width(int width) {
	this->width = width;
}

template <class SampleType>
int WaveformRenderer<SampleType>::get_width() {
	return this->width;
}

template <class SampleType>
void WaveformRenderer<SampleType>::set_height(int height) {
	this->height = height;
}

template <class SampleType>
int WaveformRenderer<SampleType>::get_height() {
	return this->height;
}

template <class SampleType>
void WaveformRenderer<SampleType>::set_scale(float scale) {
	this->scale = scale;
}

template <class SampleType>
float WaveformRenderer<SampleType>::get_scale() {
	return this->scale;
}

template <class SampleType>
void WaveformRenderer<SampleType>::set_offset(double offset) {
	if (offset < 0)
		this->offset = 0;
	else {
		if (offset >= audioData.size())
			this->offset = audioData.size() - 1;
		else
			this->offset = offset;
	}
}

template <class SampleType>
double WaveformRenderer<SampleType>::get_offset() {
	return this->offset;
}

template <class SampleType>
vector<POINT> WaveformRenderer<SampleType>::getPoints(int new_x, float new_scale, int start_x_pos) {
	vector<POINT> points;

	float new_leftSamples = (new_x < 1) ? 0 : (new_scale * static_cast<float>(new_x));
	float leftSamples = (new_x < 1) ? 0 : (scale * static_cast<float>(new_x));
	offset += (leftSamples - new_leftSamples);

	if (offset < 0.0f)
		offset = 0.0f;
	if (offset > (double)(audioData.size() - width))
		offset = (double)(audioData.size() - width);

	double numSamples = (double)(new_scale * float(width)) + offset;
	if (numSamples > double(audioData.size()))
		numSamples = (double)audioData.size();

	if (numSamples - offset > (double)MIN_SAMPLES_PER_SCREEN && audioData.size() != 0) {
		#ifdef max
		#undef max
		#endif
		#ifdef min
		#undef min
		#endif
		double sizeCorrector;
		if (SampType != _FLOAT && SampType != _INT24)
			sizeCorrector = double(((float)height / 4.0f) / std::numeric_limits<SampleType>::max());
		else {
			if (SampType == _FLOAT)
				sizeCorrector = double((float)height / 4.0f);
			else
				sizeCorrector = double((float)height / 4.0f) / 8388608;
		}
		double startSample = double(offset);
		double endSample = startSample + double(new_scale);
		bool b = true;
		if (new_scale > 50.0f) {
			for (int x = 0; x <= width; ++x) {
				if (endSample > (double)numSamples) {
					endSample = (double)numSamples;
					break;
				}

				SampleType minSample;
				SampleType maxSample;
				if (SampType != _FLOAT && SampType != _INT24) {
					minSample = std::numeric_limits<SampleType>::max();
					maxSample = std::numeric_limits<SampleType>::min();
				}
				else {
					if (SampType == _FLOAT) {
						minSample = 1;
						maxSample = -1;
					}
					else {
						minSample = 8388607;
						maxSample = -8388608;
					}
				}

				for (uint32_t i = (uint32_t)startSample; (double)(i) < endSample && i < audioData.size(); i++) {
					minSample = std::min(minSample, audioData[i]);
					maxSample = std::max(maxSample, audioData[i]);
				}
				int yMin;
				int yMax;
				if (SampType == _UINT8) {
					yMin = int( (double)((minSample - 128) * sizeCorrector)) + y_position;
					yMax = int( (double)((maxSample - 128) * sizeCorrector)) + y_position;
				}
				else {
					yMin = int( (double)(minSample * sizeCorrector)) + y_position;
					yMax = int( (double)(maxSample * sizeCorrector)) + y_position;
				}
			
				points.push_back({ x + start_x_pos, yMin });
				points.push_back({ x + start_x_pos, yMax });

				startSample = endSample;
				endSample = startSample + (double)(new_scale);
			}
		}
		else {
			if (new_scale > 1.00000000f) {
				isLower = false;
				for (int x = 0; x <= width && b; x++) {
					if (endSample > numSamples) {
						//endSample = numSamples;
						if (startSample >= (double)(audioData.size()))
							b = false;
					}
					for (int i = startSample; (double)(i) < endSample && i < audioData.size(); i++) {
						if (SampType == _UINT8) {
							points.push_back({ x + start_x_pos, (int)((audioData[i] - 128) * sizeCorrector) + y_position });
						}
						else {
							points.push_back({ x + start_x_pos, int((double)audioData[i] * sizeCorrector) + y_position });
						}
					}
					startSample = endSample;
					endSample = startSample + (double)new_scale;
				}
			}
			else {
				if (new_scale < 0.7f)
					isLower = true;
				else
					isLower = false;
				uint32_t ind = (uint32_t)startSample;
				for (int x = 0; x <= width; x++) {
					if ((uint32_t)startSample > ind) {
						if (SampType == _UINT8) {
							points.push_back({ x + start_x_pos, (int)((audioData[ind++] - 128) * sizeCorrector) + y_position });
						}
						else {
							points.push_back({ x + start_x_pos, (int)(audioData[ind++] * sizeCorrector) + y_position });
						}
					}
					startSample += (double)new_scale;
				}
			}
		}
	}
	else
		return vector<POINT>();

	scale = new_scale;
	return points;
}