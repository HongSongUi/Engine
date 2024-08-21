#pragma once
#include <map>
#include "Shader.h"
#include "ReSUtility.h"
class ShaderManager:public Singleton<ShaderManager>
{
private:
	ID3D11Device*			D3D11Device;
	ID3D11DeviceContext*	D3D11Context;
	friend class Singleton<ShaderManager>;
	std::map<std::wstring, Shader*> ShaderList;
private:
	ShaderManager() {};
public:
	void InitMgr(ID3D11Device* Device, ID3D11DeviceContext* Context);
	Shader* Load(std::wstring FineName);
	bool Release();
	~ShaderManager();
};
#define ShaderMgr ShaderManager::GetInstance()

