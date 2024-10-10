#include "SoundManager.h"
#include<io.h>
#include <tchar.h>
bool SoundManager::Init()
{
	// init Fmod system
	FMOD::System_Create(&FSystem);
	FSystem->init(32, FMOD_INIT_NORMAL, 0);
	return true;
}

bool SoundManager::Frame()
{
	FSystem->update();
	return true;
}

bool SoundManager::Release()
{
	//Resource release
	for (auto data : SoundList) 
	{
		if (data.second)
		{
			data.second->Release();
		}
	}
	FSystem->close();
	FSystem->release();
	SoundList.clear();
	return true;
}

GameSound* SoundManager::Load(std::wstring filename)
{
	//load Sound file use file name
	FMOD_RESULT hr;
	std::wstring name = GetSplitName(filename);
	auto iter = SoundList.find(name); 
	if (iter != SoundList.end()) //  find soundfile at list
	{
		return iter->second;
	}
	GameSound* NewData = new GameSound;
	hr = NewData->Load(FSystem, filename);//if there isn't file -> Load File
	if (hr == FMOD_OK) 
	{
		SoundList.insert(std::make_pair(name, NewData));
	}
	return NewData;
}

GameSound* SoundManager::Find(std::wstring name)
{
	// just find function 
	// not load
	auto iter = SoundList.find(name);
	if (iter != SoundList.end())
	{
		GameSound* sound = iter->second;
		return sound;
	}
	return nullptr;
}

void SoundManager::LoadDir(std::wstring path)
{
	// Load Directory Use path
	std::wstring dirpath = path + L"*.*";
	intptr_t handle;
	struct _wfinddata_t data;
	handle = _wfindfirst(dirpath.c_str(), &data);
	if (handle == -1L) return;

	do 
	{
		if ((data.attrib & _A_SUBDIR) && data.name[0] != '.') 
		{
			LoadDir(path + data.name + L"/");
		}
		else if (data.name[0] != '.') 
		{
			fileList.push_back(path + data.name);
		}
	} 
	while (_wfindnext(handle, &data) == 0);
	_findclose(handle);
}

void SoundManager::LoadAll(std::wstring path)
{
	// Load all files in the directory
	LoadDir(path);
	for (auto& data : fileList)
	{
		Load(data);
	}
}

std::wstring SoundManager::GetSplitName(std::wstring path)
{
	std::wstring name;
	TCHAR drive[_MAX_PATH] = { 0, };
	TCHAR dir[_MAX_PATH] = { 0, };
	TCHAR filename[_MAX_PATH] = { 0, };
	TCHAR ext[_MAX_PATH] = { 0, };
	_tsplitpath_s(path.c_str(), drive, dir, filename, ext);

	name = filename;
	name += ext;
	return name;
}


SoundManager::~SoundManager()
{
	Release();
}
