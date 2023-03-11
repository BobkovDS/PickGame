#pragma once

#include <string>
#include <vector>
#include <map>

#include <DirectXMath.h>
#include "datatypedef.h"

// FBXLoader:: Texture Type ID
enum class TEXTURE_TYPE
{
	TEXTURE_TYPE_DIFFUSE = 101,
	TEXTURE_TYPE_SPECULAR,
	TEXTURE_TYPE_NORMAL,
	TEXTURE_TYPE_TRANSPARENCY
};

class ModelFileLoader
{
public:
	enum class FileLoadStatus : char
	{
		None = 1,
		Ok,
		Failed
	};

protected:
	
	std::string m_sceneName;
	FileLoadStatus _loadStatus;

	template<typename T, typename C>
	const T* data(int i, const C& c) const
	{
		auto it = c.begin();
		std::advance(it, i);

		if (it != c.end())
			return &it->second;
		else
			return nullptr;
	}

	template<typename T>
	const T* dataVector(int i, const std::vector<T>& c) const
	{
		auto it = c.begin();
		std::advance(it, i);

		if (it != c.end())
			return &(*it);
		else
			return nullptr;
	}

public:

	ModelFileLoader() :_loadStatus(FileLoadStatus::Ok) {}

	virtual void loadFile(const std::string& skinFileName) = 0;
	virtual void prepare() {};
	virtual u32 meshCount() const = 0;
	virtual u32 materialsCount() const = 0;
	virtual u32 instancesCount(u32 meshId =0) const = 0;
	virtual u32 texturesCount() const = 0;
	virtual ~ModelFileLoader() {}

	bool IsSuccessful() const { return _loadStatus == FileLoadStatus::None; }
	std::string sceneName()const { return m_sceneName; }
};

class ModelDataConsumer
{
public:
	virtual void consume(ModelFileLoader* fileLoader) = 0;
	virtual bool writeToDevice() = 0;
	virtual ~ModelDataConsumer(){}
};