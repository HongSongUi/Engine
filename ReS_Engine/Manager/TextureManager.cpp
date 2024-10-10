#include "TextureManager.h"
#include <tchar.h>
void TextureManager::InitMgr(ID3D11Device* Device, ID3D11DeviceContext* Context)
{
	//init device, context
    D3D11Device = Device;
    D3D11Context = Context;
}

Texture* TextureManager::Load(std::wstring name)
{
	//Load texture file , use file name
	HRESULT hr;

	auto iter = TextureList.find(name); 
	if (iter != TextureList.end()) //if find file
	{
		return iter->second; // return file
	}
	Texture* NewData = new Texture;
	hr = NewData->Load(D3D11Device, D3D11Context, name); // if there isn't file -> Load texture file
	if (hr == S_OK) 
	{
		TextureList.insert(std::make_pair(name, NewData));
		return NewData;
	}
	else 
	{
		NewData->Release();
		delete  NewData;
		NewData = nullptr;
		return nullptr;
	}
}

Texture* TextureManager::Find(std::wstring name)
{
	auto iter = TextureList.find(name);
	if (iter != TextureList.end())
	{
		return iter->second;
	}
	return nullptr;
}

std::wstring TextureManager::GetSplitName(std::wstring path)
{
	std::wstring name;
	TCHAR drive[MAX_PATH] = { 0, };
	TCHAR dir[MAX_PATH] = { 0, };
	TCHAR filename[MAX_PATH] = { 0, };
	TCHAR ext[MAX_PATH] = { 0, };
	std::wstring tmp;
	_tsplitpath_s(path.c_str(), drive, dir, filename, ext);
	tmp = ext;
	if (tmp == L".tga" || tmp == L".TGA") 
	{
		tmp = L".dds";
	}

	name = filename;
	name += tmp;
	return name;
}

bool TextureManager::Release()
{
	for (auto data : TextureList)
	{
		Texture* tex = data.second;
		if (tex)
		{
			tex->Release();
			delete tex;
			tex = nullptr;
		}
	}
	TextureList.clear();
	return true;
}


TextureManager::~TextureManager()
{
	Release();
}
