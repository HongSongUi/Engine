#pragma once
#include <map>
#include <list>
#include "GameSound.h"
#include "ReSUtility.h"
class SoundManager: public Singleton<SoundManager>
{
public:
	FMOD::System* FSystem;
	std::list<std::wstring> fileList;
private:
	friend class Singleton<SoundManager>;
	std::map<std::wstring, GameSound*> SoundList;
public:
	GameSound* Load(std::wstring filename);
	GameSound* Find(std::wstring name);
	void LoadDir(std::wstring path);
	void LoadAll(std::wstring path);
	std::wstring GetSplitName(std::wstring path);
public:
	bool Init();
	bool Frame();
	bool Release();
private:
	SoundManager() {};
public:
	~SoundManager();
};
#define SoundMgr SoundManager::GetInstance()

