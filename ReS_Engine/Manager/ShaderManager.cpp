#include "ShaderManager.h"

void ShaderManager::InitMgr(ID3D11Device* Device, ID3D11DeviceContext* Context)
{
	//Init Device, Context
	D3D11Device = Device;
	D3D11Context = Context;
}

Shader* ShaderManager::Load(std::wstring FileName)
{
	//Load Shader File Use File Name
	HRESULT hr;
	auto iter = ShaderList.find(FileName); // find shader file in the list
	if (iter != ShaderList.end()) // if find shader
	{
		return iter->second;
	}
	Shader* data = new Shader; 
	hr = data->Load(FileName, D3D11Device); // if there isn't shader-> Create
	if (hr == S_OK) 
	{
		ShaderList.insert(std::make_pair(FileName, data)); // insert in to list
		return data;
	}
	else
	{
		return nullptr;
	}
	
}

bool ShaderManager::Release()
{
	//Resource release
	for (auto obj : ShaderList)
	{
		Shader* data = obj.second;
		if (data) 
		{
			data->Release();
			delete data;
		}
	}
	ShaderList.clear();
	return true;
}

ShaderManager::~ShaderManager()
{
	Release();
}
