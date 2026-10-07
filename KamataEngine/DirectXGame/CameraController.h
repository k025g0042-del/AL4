#pragma once
#include "KamataEngine.h"

class Player;

/// <summary>
/// カメラコントローラ
/// </summary>
class CameraController {
private:
	//矩形
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	//カメラ
	KamataEngine::Camera camera_;

	//追従対象のポインタ
	Player* target_ = nullptr;
	
	//追従対象とカメラ座標の差(オフセット)
	KamataEngine::Vector3 targetOffset_{0, 0, -15.0f};

	//カメラ移動範囲
	Rect movableArea_ = {0, 100, 0, 100};

	//カメラの目標座標
	KamataEngine::Vector3 markPosition;

	//座標補間割合
	static inline const float kInterpolationRate = 0.8f;

	// 速度掛け率
	static inline const float kVelocityBias = 10.0f;

	static inline const Rect margin = {-10.0f, 10.0f, -5.0f, 5.0f};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	KamataEngine::Camera& GetCamera() { return camera_; }

	void SetMovableArea(Rect area) { movableArea_ = area; }
};
