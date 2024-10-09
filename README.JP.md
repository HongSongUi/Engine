# 3D Game Engine

<img src ="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white"> <img src ="https://img.shields.io/badge/Direct_X-006600?style=for-the-badge&logo=directx&logoColor=black"> <img src ="https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white"> <img src="https://img.shields.io/badge/fmod-000000?style=for-the-badge&logo=fmod&logoColor=white">

## ゲームエンジン概要

このゲームエンジンは、DirectXとC++を使用して製作された2Dおよび3Dゲームエンジンです。

## 主要機能

- **キーボード入力**: 様々なキーボード入力を提供。

- **サウンド機能**: Fmodを使用してサウンド機能を提供。

- **2D/3D衝突処理**: 2Dおよび3D衝突機能をサポートして物体間の相互作用を処理。

- **カメラ機能**:2D及び3Dカメラ機能を提供し、ツール開発に必要なデバッグカメラ機能を提供。

- **レンダリング機能**:RenderToTexture機能により、ポストプロセッシング効果を実現可能。

- **効率的なリソース管理**: Singleton パターンを使用してテクスチャ、サウンド、シェーダーを効率的に管理。

- **FPS及びDeltaTime提供**:FPS計算機能とゲーム内DeltaTimeを提供し、正確な時間ベースのアップデートが可能。

- **マウスピッキング**:マウスピッキング機能により3D空間でオブジェクトを選択可能。

- **Frustum Culling**:SpaceDivisionとFrustumを活用してFrustum Culling機能を提供。

- **FBXファイルロード**: FBXファイルをロードする機能により、3Dモデルを簡単にインポート可能。

## How To Use

1. クラス生成:GameCoreを継承したクラスを生成します。 このクラスでInit、Frame、Render、Release関数を実装します。
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
		    // 初期化コード作成
		    return true;
		}
		
		bool Sample::Frame()
		{
		    // フレーム処理コード作成
		    return true;
		}
		
		bool Sample::Render() 
		{
		    // レンダリングコード作成
		    return true;
		}
		
		bool Sample::Release()
		{
		    // リソース解除コードの作成
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

2.WWinMain関数生成:上記のコードのようにSample.cppにWWinMain関数を生成してプログラムの開始点を設定します。

* エンジン構造の簡単なシーケンス図
  
<img src ="https://github.com/user-attachments/assets/4a8e8dbf-0bff-422c-be88-7059ea0b83e7">

## Projects Developed Using This Engine

[Megaman X5](https://github.com/HongSongUi/RockMan) [Map Tool](https://github.com/HongSongUi/MapTool)
