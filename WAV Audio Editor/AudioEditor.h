#pragma once
#include <vector>
#include <list>
#include <string>
#include <unordered_map>
#include <functional>
enum EditorActions {
	_cancel,
	_undo,
	_cut,
	_copy,
	_insert,
	_delete_part,
	_delete_all_except_of_part
};
using namespace std;
template<class SampleType>
class AudioEditor
{
private:
	vector<SampleType> audioData;
	enum methods
	{
		__delete,
		__insert,
		__cut,
	};
	//int delete_it = 0;
	struct Action {
		unsigned char method;
		//unsigned short startX;
		float scale;
		float offset;
		uint64_t from;
		uint64_t to;
	};
	list<vector<SampleType>> deletedVecs;
	//list<vector<float>> insertedVecs;
	vector<SampleType> copyVec;
	list<Action> lastVectUsed;
	list<Action> lastUndo;
	//vector<float> __delete_part_of_Audio(unsigned short startX, uint64_t length, uint64_t from, uint64_t to, unsigned char c);
	vector<SampleType>* __insert_to_Audio(const float scale, const float offset, uint64_t place, unsigned char c);

public:
	AudioEditor() = delete;
	AudioEditor(vector<SampleType> &audioData);

	vector<SampleType>& GetData();
	void SetData(vector<SampleType> &audioData);

	vector<SampleType>* delete_part_of_Audio(const float scale, const float offset, uint64_t from, uint64_t to);
	vector<SampleType>* insert_to_Audio(const float scale, const float offset, uint64_t place);
	vector<SampleType>* copy_part_of_Audio(const float scale, const float offset, uint64_t from, uint64_t to);
	vector<SampleType>* cut_part_of_Audio(const float scale, const float offset, uint64_t from, uint64_t to);
	vector<SampleType>* delete_all_out_of_selection(const float scale, const float offset, uint64_t from, uint64_t to, uint64_t width);
	vector<SampleType>* cancel();
	vector<SampleType>* undo();

	//int get_sizeofType();
	int typeSize = -1;
	using DataType = SampleType;
};

bool isCancel = false;
bool isUndo;
#define max_min uint64_t max = Max(audioData.size(), length); \
				uint64_t min = Min(audioData.size(), length); \
				float scale = max / min;
#define from_to from -= startX; \
				to -= startX; \
				from *= scale; \
				to *= scale;
#define save_action(data, action, startX, length, sc, from, to, vecName) \
					this->vecName##Vecs.push_back(data); \
					this->lastVectUsed.push_back({ __##action, startX, length, sc, from, to });
#define cancel_undo(v_to_del, v_to_add) \
                        if (!last##v_to_del.empty()) { \
							Action act = last##v_to_del.back(); \
							last##v_to_add.push_back(act); \
							last##v_to_del.pop_back(); \
							switch (act.method)\
							{\
							case __insert:\
								v = delete_part_of_Audio(act.startX, act.length, act.from, act.to); \
								break;\
							case __delete:\
							case __cut:\
								v = insert_to_Audio(act.startX, act.length, act.from); \
								break;\
							default:\
								break; \
							} \
						}
uint64_t Max(uint64_t a, uint64_t b) {
	return (a > b) ? a : b;
}
uint64_t Min(uint64_t a, uint64_t b) {
	return (a > b) ? b : a;
}
template<class SampleType>
AudioEditor<SampleType>::AudioEditor(vector<SampleType>& audioData)
{
	this->audioData = audioData;
}
template<class SampleType>
vector<SampleType>& AudioEditor<SampleType>::GetData()
{
	return this->audioData;
}
template<class SampleType>
void AudioEditor<SampleType>::SetData(vector<SampleType>& audioData)
{
	this->audioData = audioData;
}
template<class SampleType>
vector<SampleType>* AudioEditor<SampleType>::delete_part_of_Audio(const float scale, const float offset, uint64_t from, uint64_t to)
{
	if (!audioData.empty()) {
		if (isCancel && !isUndo) {
			lastUndo.clear();
			isCancel = false;
		}
		this->lastVectUsed.push_back({ __delete, scale, offset, from, to });
		from = round(offset + (float)from * scale);
		to = round(offset + (float)to * scale);
		if (from < audioData.size()) {
			if (typeSize != -1) {
				int diff = to % typeSize;
				if (diff != 0)
					to += diff;
				diff = from % typeSize;
				if (diff != 0)
					from -= typeSize - diff;
				if (to >= audioData.size())
					to = audioData.size() - 1;
			}
			if (from < 0)
				from = 0;
			vector<SampleType> copy;
			if (to >= audioData.size()) {
				copy.assign(audioData.begin() + from, audioData.end());
				audioData.erase(audioData.begin() + from, audioData.end());
			}
			else {
				copy.assign(audioData.begin() + from, audioData.begin() + to);
				audioData.erase(audioData.begin() + from, audioData.begin() + to);
			}
			//uint64_t l2 = audioData.size();
			//scale = (float)l2 / float(l1);
			//length = std::round(length * scale);
			//save_action(copy, delete, startX, length, scale, fr, t, deleted)
			this->deletedVecs.push_back(copy);
		}
	}
	return &this->audioData; 
}

template<class SampleType>
vector<SampleType>* AudioEditor<SampleType>::copy_part_of_Audio(const float scale, const float offset, uint64_t from, uint64_t to)
{
	if (!audioData.empty()) {
		//max_min
			//from_to
		from = round(offset + (float)from * scale);
		to = round(offset + (float)to * scale);
		if (from < audioData.size()) {
			if (typeSize != -1) {
				int diff = to % typeSize;
				if (diff != 0)
					to += diff;
				diff = from % typeSize;
				if (diff != 0)
					from -= typeSize - diff;
				if (to >= audioData.size())
					to = audioData.size() - 1;
			}
			if (from < 0)
				from = 0;
			if (to >= audioData.size()) {
				copyVec = vector<SampleType>(audioData.begin() + from, audioData.end());
			}
			else {
				copyVec = vector<SampleType>(audioData.begin() + from, audioData.begin() + to);
			}
		}
	}
	return &copyVec;
}

template<class SampleType>
vector<SampleType>* AudioEditor<SampleType>::cut_part_of_Audio(const float scale, const float offset, uint64_t from, uint64_t to)
{
	if (!audioData.empty()) {
		if (isCancel && !isUndo) {
			lastUndo.clear();
			isCancel = false;
		}
		//
		this->lastVectUsed.push_back({ __delete, scale, offset, from, to });
		from = round(offset + (float)from * scale);
		to = round(offset + (float)to * scale);
		//max_min
			//from_to
		if (from < audioData.size()) {
			if (typeSize != -1) {
				int diff = to % typeSize;
				if (diff != 0)
					to += diff;
				diff = from % typeSize;
				if (diff != 0)
					from -= typeSize - diff;
				if (to >= audioData.size())
					to = audioData.size() - 1;
			}
			if (from < 0)
				from = 0;
			if (to >= audioData.size()) {
				copyVec = vector<SampleType>(audioData.begin() + from, audioData.end());
				this->audioData.erase(this->audioData.begin() + from, this->audioData.end());
			}
			else {
				copyVec = vector<SampleType>(audioData.begin() + from, audioData.begin() + to);
				this->audioData.erase(this->audioData.begin() + from, this->audioData.begin() + to);
			}		
			this->deletedVecs.push_back(copyVec);
		}
	}
	return &this->audioData;
}

template<class SampleType>
vector<SampleType>* AudioEditor<SampleType>::delete_all_out_of_selection(const float scale, const float offset, uint64_t from, uint64_t to, uint64_t width)
{
	vector<SampleType>* audioData = delete_part_of_Audio(scale, offset, to, width);
	audioData = delete_part_of_Audio(scale, offset, 0, from);
	return audioData;
}

template<class SampleType>
vector<SampleType>* AudioEditor<SampleType>::cancel()
{
	if (!lastVectUsed.empty()) {
		isCancel = false;
		vector<SampleType>* v = NULL;
		Action act = lastVectUsed.back();
		lastUndo.push_back(act);
		lastVectUsed.pop_back();
		switch (act.method)
		{
		case __insert:
			v = delete_part_of_Audio(act.scale, act.offset, act.from, act.to);
			//insertedVecs.pop_back();
			break;
		case __delete:
		case __cut:
			v = __insert_to_Audio(act.scale, act.offset, act.from, 1);
			deletedVecs.pop_back();
			break;
		default:
			break;
		}

		act = lastVectUsed.back();
		lastVectUsed.pop_back();
		switch (act.method) {
		case __delete:
			deletedVecs.pop_back();
			break;
		case __insert:
			//insertedVecs.pop_back();
			break;
		default:
			break;
		}
		isCancel = true;
		return v;
	}
	return &audioData;
}

template<class SampleType>
vector<SampleType>* AudioEditor<SampleType>::undo()
{
	if (!lastUndo.empty()) {
		isUndo = true;
		vector<SampleType>* v = NULL;
		Action act = lastUndo.back();
		lastUndo.pop_back();
		switch (act.method)
		{
		case __insert:
			//act.length = std::round(act.length / act.scale);
			v = insert_to_Audio(act.scale, act.offset, act.from);
			break;
		case __delete:
		case __cut:
			//act.length = std::round(act.length / act.scale);
			v = delete_part_of_Audio(act.scale, act.offset, act.from, act.to);
			break;
		default:
			break;
		}
		isUndo = false;
		return v;
	}
	else
		return &audioData;
}
/*template<class SampleType>
int AudioEditor<SampleType>::get_sizeofType()
{
	return sizeof(SampleType);
}*/
template<class SampleType>
vector<SampleType>* AudioEditor<SampleType>::__insert_to_Audio(const float scale, const float offset, uint64_t place, const unsigned char c) {
	uint64_t fr = place;
	//max_min

	//place -= startX;
	//place *= scale;
	if (isCancel && !isUndo) {
		lastUndo.clear();
		isCancel = false;
	}
	place = round(offset + (float)place * scale);
	vector<SampleType> copy;
	if (c)
		copy = deletedVecs.back();
	else
		copy = copyVec;
	if (!copy.empty() && place >= 0) {
		if (place > audioData.size()) {
			if (audioData.empty())
				place = 0;
			else
				place = audioData.size();
		}
		if (typeSize != -1) {
			int diff = place % typeSize;
			if (diff != 0)
				place -= typeSize - diff;
		}
		if (place < 0)
			place = 0;

		this->audioData.insert(this->audioData.begin() + place, copy.begin(), copy.end());

		uint64_t t = round(float(fr) + float(copy.size()) / scale);
		this->lastVectUsed.push_back({ __insert, scale, offset, fr, t });

	}
	return &this->audioData;
}
template<class SampleType>
vector<SampleType>* AudioEditor<SampleType>::insert_to_Audio(const float scale, const float offset, uint64_t place)
{
	return __insert_to_Audio(scale, offset, place, 0);
}
