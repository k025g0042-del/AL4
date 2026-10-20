#pragma once
#include "AABBClass.h"
#include "KamataEngine.h"
#include <numbers>

class MapChipField;
class Enemy;
class ShieldEnemy;

// 左右
enum class LRDirection {
	kRight,
	kLeft,
};

/// <summary>
/// 自キャラ
/// </summary>
class Player {
private:
	// 角
	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,

		kNumCorner // 要素数
	};

	// マップとの当たり判定情報(天井衝突フラグ/着地フラグ/壁接触フラグ/移動量)
	struct CollisionMapInfo {
		bool ceilingHit = false;
		bool landing = false;
		bool wallTouch = false;
		KamataEngine::Vector3 movement;
	};

	// 左右の自キャラの角度テーブル
	static inline const float destinationRotationYTable[] = {
	    std::numbers::pi_v<float> * 4.0f / 2.0f,
	    std::numbers::pi_v<float> / 1.0f,
	};

	// 振る舞い
	// 通常状態、攻撃中、ノックバック
	enum class Behavior {
		kUnknown,
		kRoot,
		kAttack,
		kKnockback,
	};

	// 攻撃フェーズ型
	// 溜め、突進、余韻
	enum class AttackPhase {
		kReservoir,
		kRush,
		kReverberation,
	};

	// ノックバックフェーズ型
	enum class KnockbackPhase {
		kBlowAway,
		kRebuild,
	};

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// 加速度
	static inline const float kAcceleration = 0.01f;

	// 速度減衰率
	static inline const float kAttenuation = 0.02f;

	// 速度制限
	static inline const float kLimitRunSpeed = 5.0f;

	// 向き
	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;

	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 旋回時間
	static inline const float kTimeTurn = 0.3f;

	// 接地状態フラグ
	bool onGround_ = true;

	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.049f;

	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 10.0f;

	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 0.5f;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 空白
	static inline const float kBlank = 0.01f;
	static inline const float kBlankBottom = -0.01f;

	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.01f;

	// 壁接触時の速度減衰率
	static inline const float kAttenuationWall = 0.1f;

	// デスフラグ
	bool isDead_ = false;

	// 振る舞い
	Behavior behavior_ = Behavior::kRoot;

	// 次の振る舞いをリクエスト
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// 攻撃ギミックの経過時間カウンター
	uint32_t attackParameter_ = 0;

	// 現在の攻撃フェーズ
	AttackPhase attackPhase_;

	KamataEngine::Vector3 attackVelocity_ = {1.0f, 0.0f, 0.0f};

	// ノックバックリクエストフラグ
	bool isKnockbackRequest_ = false;

	// ノックバックの経過時間カウンター
	float knockbackParameter_ = 0;

	// 現在のノックバックフェーズ
	KnockbackPhase knockbackPhase_;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <param name="camera">カメラ</param>
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 移動入力
	/// </summary>
	void MoveInput();

	/// <summary>
	/// 旋回制御
	/// </summary>
	void TurningControl();

	/// <summary>
	/// マップ衝突判定
	/// </summary>
	/// <param name="info"></param>
	void MapCollisionDecision(CollisionMapInfo& info);

	void MapCollisionDecisionTop(CollisionMapInfo& info);

	void MapCollisionDecisionBottom(CollisionMapInfo& info);

	void MapCollisionDecisionRight(CollisionMapInfo& info);

	void MapCollisionDecisionLeft(CollisionMapInfo& info);

	/// <summary>
	/// 判定結果を反映して移動させる
	/// </summary>
	/// <param name="info"></param>
	void JudgmentResultMovement(const CollisionMapInfo& info);

	/// <summary>
	/// 天井に接触している場合の処理
	/// </summary>
	/// <param name="info"></param>
	void CeilingHitProcess(const CollisionMapInfo& info);

	/// <summary>
	/// 壁に接触している場合の処理
	/// </summary>
	/// <param name="info"></param>
	void WallTouch(const CollisionMapInfo& info);

	/// <summary>
	/// 接地状態の切り替え処理
	/// </summary>
	/// <param name="info"></param>
	void GroundSwitching(const CollisionMapInfo& info);

	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	const KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	KamataEngine::Vector3 GetWorldPosition();

	/// <summary>
	/// AABBを取得
	/// </summary>
	/// <returns></returns>
	AABBClass::AABB GetAABB();

	/// <summary>
	/// 衝突応答
	/// </summary>
	/// <param name="enemy"></param>
	void OnCollision(const Enemy* enemy);

	/// <summary>
	/// 衝突応答
	/// </summary>
	/// <param name="enemy"></param>
	void OnCollision(const ShieldEnemy* enemy);

	/// <summary>
	/// デスフラグのgetter
	/// </summary>
	/// <returns></returns>
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// 通常行動更新
	/// </summary>
	void BehaviorRootUpdate(CollisionMapInfo& info);

	/// <summary>
	/// 通常行動初期化
	/// </summary>
	void BehaviorRootInitialize();

	/// <summary>
	/// 攻撃行動更新
	/// </summary>
	void BehaviorAttackUpdate(CollisionMapInfo& info);

	/// <summary>
	/// 攻撃行動初期化
	/// </summary>
	void BehaviorAttackInitialize();

	/// <summary>
	/// ノックバック行動更新
	/// </summary>
	void BehaviorKnockbackUpdate(CollisionMapInfo& info);

	/// <summary>
	/// ノックバック行動初期化
	/// </summary>
	void BehaviorKnockbackInitialize();

	/// <summary>
	/// 攻撃しているか
	/// </summary>
	/// <returns></returns>
	bool IsAttack();

	float GetWorldTransformX() const { return worldTransform_.translation_.x; }
	float GetWorldTransformY() const { return worldTransform_.translation_.y; }
	float GetWorldTransformZ() const { return worldTransform_.translation_.z; }

	LRDirection GetLRDirection() const { return lrDirection_; }

	void KnockbackRequest() { isKnockbackRequest_ = true; }
};
