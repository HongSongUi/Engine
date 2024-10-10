#pragma once
#include <map>
#include "../DirectX/Shader.h"
#include "../Utility/ReSUtility.h"
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
	Shader* Load(std::wstring FileName);
	bool Release();
	~ShaderManager();
};
#define ShaderMgr ShaderManager::GetInstance()

