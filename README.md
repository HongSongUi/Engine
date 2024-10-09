## 3D Game Engine

<img src ="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white"> <img src ="https://img.shields.io/badge/Direct_X-006600?style=for-the-badge&logo=directx&logoColor=black"> <img src ="https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white"> <img src="https://img.shields.io/badge/fmod-000000?style=for-the-badge&logo=fmod&logoColor=white">

# 게임 엔진 개요

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
