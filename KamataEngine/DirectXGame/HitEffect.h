#pragma once
#include "KamataEngine.h"

/// <summary>
/// ヒット演出用エフェクト
/// </summary>
class HitEffect {
private:
	// 状態遷移
	enum Behavior {
		kUnknown,
		kSpread,
		kFeadOut,
		kLapse,
	};

	// モデル(借りてくる用)
	static KamataEngine::Model* model_;

	// カメラ(借りてくる用)
	static KamataEngine::Camera* camera_;

	// 円のワールドトランスフォーム
	KamataEngine::WorldTransform circleWorldTransform_;

	// 楕円のワールドトランスフォーム
	std::array<KamataEngine::WorldTransform, 2> ellipseWorldTransform_;

	float random_ = 0.0f;

	// 振る舞い
	Behavior behavior_ = Behavior::kSpread;

	// 次の振る舞いをリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// 秒数経過タイマー
	float timer_ = 0;

	// デスフラグ
	bool isDead_ = false;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const KamataEngine::Vector3& occurrencePosition);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// インスタンス作成と初期化
	/// </summary>
	/// <returns></returns>
	static HitEffect* Create(const KamataEngine::Vector3& occurrencePosition);

	// 静的メンバ変数のセッター
	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

	/// <summary>
	/// デスフラグのgetter
	/// </summary>
	/// <returns></returns>
	bool IsDead() const { return isDead_; }
};
