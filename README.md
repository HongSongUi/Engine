# 3D Game Engine

<img src ="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white"> <img src ="https://img.shields.io/badge/Direct_X-006600?style=for-the-badge&logo=directx&logoColor=black"> <img src ="https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white"> <img src="https://img.shields.io/badge/fmod-000000?style=for-the-badge&logo=fmod&logoColor=white">

[日本の方はこちらへ](./README.JP.md)


## 게임 엔진 개요

이 게임 엔진은 DirectX와 C++를 사용하여 제작된 2D 및 3D 게임 엔진입니다.

## 주요 기능

- **키보드 입력 지원**: 다양한 키보드 입력 기능을 지원.
  
- **사운드 기능**: Fmod를 사용하여 사운드 기능을 제공.
  
- **2D/3D 충돌 처리**: 2D 및 3D 충돌 기능을 지원하여 물체 간 상호작용을 처리.
  
- **카메라 기능**: 2D 및 3D 카메라 기능을 제공하며, 툴 개발에 필요한 디버그 카메라 기능도 지원.
  
- **렌더링 기능**: RenderToTexture 기능을 통해 포스트 프로세싱 효과를 구현 가능.
  
- **효율적인 리소스 관리**: Singleton 패턴을 사용하여 텍스처, 사운드, 셰이더를 효율적으로 관리.
  
- **FPS 및 DeltaTime 지원**: FPS 계산 기능과 게임 내 DeltaTime을 지원하여 정확한 시간 기반 업데이트가 가능.
  
- **마우스 피킹**: 마우스 피킹 기능을 통해 3D 공간에서 객체를 선택 가능.
  
- **프러스텀 컬링**: SpaceDivision과 Frustum을 활용하여 프러스텀 컬링 기능을 지원.
  
- **FBX 파일 로드**: FBX 파일을 로드하는 기능을 통해 3D 모델을 쉽게 임포트 가능.

## How To Use

1. 클래스 생성: GameCore를 상속받은 클래스를 생성합니다. 이 클래스에서 Init, Frame, Render, Release 함수를 구현합니다.
	* Sample.h
	  ```cpp
	  #include "GameCore.h"
	  class Sample : public GameCore
	  {
	
	  public:
		bool Init();
		bool Frame();
		bool Render();
		bool Release();
	  };
  
	* Sample.cpp
   		```cpp
     	#include "Sample.h"
		 bool Sample::Init() 
		{
		    // 초기화 코드 작성
		    return true;
		}
		
		bool Sample::Frame()
		{
		    // 프레임 처리 코드 작성
		    return true;
		}
		
		bool Sample::Render() 
		{
		    // 렌더링 코드 작성
		    return true;
		}
		
		bool Sample::Release()
		{
		    // 자원 해제 코드 작성
		    return true;
		}
		
		int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
		    _In_opt_ HINSTANCE hPrevInstance,
		    _In_ LPWSTR lpCmdLine,
		    _In_ int nCmdShow)
		{
	
		    Sample MyWin;
		    MyWin.SetWindow(hInstance, L"test", 1000, 700);
		    MyWin.Run();
		
		    return 1;
		}

2. wWinMain 함수 생성: 위의 코드처럼 Sample.cpp에 wWinMain 함수를 생성하여 프로그램의 시작점을 설정합니다.

* 엔진 구조의 간단한 시퀀스 다이어그램
  
<img src ="https://github.com/user-attachments/assets/4a8e8dbf-0bff-422c-be88-7059ea0b83e7">

## Projects Developed Using This Engine

[Megaman X5](https://github.com/HongSongUi/RockMan) [Map Tool](https://github.com/HongSongUi/MapTool)
