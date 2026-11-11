#pragma once
#include "AABBClass.h"
#include "KamataEngine.h"

class Player;
class GameScene;

/// <summary>
/// 敵
/// </summary>
class Enemy {
private:
	enum class Behavior {
		kUnknown,
		kWalk,
		kDeath,
	};

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.01f;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = 0;

	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 45;

	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 2.0f;

	// 経過時間
	float walkTimer_ = 0.0f;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.9f;
	static inline const float kHeight = 0.9f;

	// デスフラグ
	bool isDead_ = false;

	//振る舞い
	Behavior behavior_ = Behavior::kWalk;

	// 次の振る舞いをリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	//デスアニメーションタイマー
	float animationTimer_ = 0.0f;

	//コリジョン無効フラグ
	bool isCollisionDisabled_ = false;

	GameScene* gameScene_ = nullptr;

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	KamataEngine::Vector3 GetWorldPosition();

	/// <summary>
	/// AABBを取得
	/// </summary>
	/// <returns></returns>
	AABBClass::AABB GetAABB();

	/// <summary>
	/// 衝突応答
	/// </summary>
	/// <param name="player"></param>
	void OnCollision(Player* player);

	/// <summary>
	/// デスフラグのgetter
	/// </summary>
	/// <returns></returns>
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// 歩行行動更新
	/// </summary>
	void BehaviorWalkUpdate();

	/// <summary>
	/// 歩行行動初期化
	/// </summary>
	void BehaviorWalkInitialize();

	/// <summary>
	/// デス演出更新
	/// </summary>
	void BehaviorDeathUpdate();

	/// <summary>
	/// デス演出初期化
	/// </summary>
	void BehaviorDeathInitialize();

	bool IsCollisionDisabled() { return isCollisionDisabled_; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
};
