#include "ShaderManager.h"

void ShaderManager::InitMgr(ID3D11Device* Device, ID3D11DeviceContext* Context)
{
	D3D11Device = Device;
	D3D11Context = Context;
}

Shader* ShaderManager::Load(std::wstring FineName)
{
	HRESULT hr;
	auto iter = ShaderList.find(FineName);
	if (iter != ShaderList.end())
	{
		return iter->second;
	}
	Shader* data = new Shader;
	hr = data->Load(FineName, D3D11Device);
	if (hr == S_OK) 
	{
		ShaderList.insert(std::make_pair(FineName, data));
		return data;
	}
	else
	{
		return nullptr;
	}
	
}

bool ShaderManager::Release()
{
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
