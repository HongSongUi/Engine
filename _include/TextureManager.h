#pragma once
#include <map>
#include "ReSUtility.h"
#include "Texture.h"
class TextureManager:public Singleton<TextureManager>
{
private:
	ID3D11Device* D3D11Device = nullptr;
	ID3D11DeviceContext* D3D11Context = nullptr;
private:
	friend class Singleton<TextureManager>;
	std::map<std::wstring, Texture*> TextureList;
public:
	Texture* Load(std::wstring name);
	Texture* Find(std::wstring name);
	std::wstring GetSplitName(std::wstring path);

	bool Release();
	void InitMgr(ID3D11Device* Device, ID3D11DeviceContext* Context);
private:
	TextureManager() {};
public:
	~TextureManager();
};
#define TextureMgr TextureManager::GetInstance()
