#pragma once

#include <string>
#include <vector>

enum class UniformType
{
	Bool = 0,
	Int,
	Uint,
	Float,
	fVec2,
	fVec3,
	fVec4,
	Mat2,
	Mat3,
	Mat4,
	Sampler2D,
	Invalid
};

class Uniform
{
public:

	struct SamplerData {
		size_t tex_id;
		size_t resource_id;
		char tex_path[256];
	};

	Uniform()
		: type(UniformType::Invalid), name(), data(nullptr), uniformID(-1)
	{}


	Uniform(std::string name, UniformType type)
		: type(type), data(nullptr), name(name), uniformID(NULL)
	{
		setEmptyData();
	}
	~Uniform()
	{
		if (data != nullptr)
		{
			delete[] data;
			data = nullptr;
		}
	}

	void sendToShader(const uint16_t shaderProgram, int& textureId);

	template <class T>
	void setData(const T& value, const UniformType type)
	{
		setType(type);

		//If it's already a value we need to free the data
		if (data)
			delete[](data);

		//Reserving memory and copy the value
		dataSize = sizeof(T);
		data = new unsigned char[dataSize];
		memcpy(data, &value, sizeof(T));
	}

	void setEmptyData();

	template <class T>
	void getData(T* data)
	{
		data = (T*)this->data;
	}
	template <class T>
	T getData()
	{
		return *(T*)data;
	}
	template <class T>
	T* getPtrData()
	{
		return (T*)data;
	}
	inline UniformType getType() const { return type; }
	inline void setType(UniformType val) { type = val; }

private:

	UniformType type;
	//Using unsigned char as a byte
	unsigned char* data = nullptr;
	size_t dataSize = 0;
	uint16_t uniformID;

public:

	std::string name;

};