#define NOMINMAX
#include "Player.h"
#include "Calculation.h"
#include "Easing.h"
#include "MapChipField.h"
#include "WorldTransformUpDate.h"
#include "cassert"
#include <algorithm>

using namespace KamataEngine;

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// NULLポイントチェック
	assert(model);

	model_ = model;

	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	WorldTransformUpdate(worldTransform_);
}

void Player::Update() {
	// 外部からのノックバックリクエストを処理
	if (isKnockbackRequest_) {
		behaviorRequest_ = Behavior::kKnockback;

		// フラグをリセット
		isKnockbackRequest_ = false;
	}

	// Behavior遷移
	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_;

		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:

			// ルートビヘイビアの初期化
			BehaviorRootInitialize();

			break;
		case Behavior::kAttack:

			// 攻撃ビヘイビアの初期化
			BehaviorAttackInitialize();

			break;
		case Behavior::kKnockback:

			// ノックバックビヘイビアの初期化
			BehaviorKnockbackInitialize();

			break;
		}

		// 振る舞いリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;

	switch (behavior_) {
	case Player::Behavior::kRoot:

		// ルートビヘイビアの更新
		BehaviorRootUpdate(collisionMapInfo);

		break;
	case Player::Behavior::kAttack:

		// 攻撃ビヘイビアの更新
		BehaviorAttackUpdate(collisionMapInfo);

		break;

	case Behavior::kKnockback:

		// ノックバックビヘイビアの更新
		BehaviorKnockbackUpdate(collisionMapInfo);

		break;
	}

	// マップ衝突チェック
	MapCollisionDecision(collisionMapInfo);

	// 移動
	JudgmentResultMovement(collisionMapInfo);

	// 天井接触処理
	CeilingHitProcess(collisionMapInfo);

	// 壁接触処理
	WallTouch(collisionMapInfo);

	// 接地状態の切り替え
	GroundSwitching(collisionMapInfo);

	// 行列の更新
	WorldTransformUpdate(worldTransform_);
}

void Player::Draw() {

	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}

void Player::MoveInput() {
	// 移動入力
	// 接地状態
	if (onGround_) {

		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x += kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = static_cast<float>(LRDirection::kLeft);
					turnTimer_ = kTimeTurn;
				}

			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x -= kAcceleration;

				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = static_cast<float>(LRDirection::kRight);
					turnTimer_ = kTimeTurn;
				}
			}

			// 加速/減速
			velocity_.x += acceleration.x;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}

		// ジャンプ
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_.x += 0.0f;
			velocity_.y += kJumpAcceleration;
			velocity_.z += 0.0f;
		}

	} else {
		// 空中

		// 落下速度
		velocity_.x += 0.0f;
		velocity_.y += -kGravityAcceleration;
		velocity_.z += 0.0f;

		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::TurningControl() {
	// 旋回制御
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;

		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		float easingT = Easing::InOutQuad(turnTimer_ / kTimeTurn);
		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = (1.0f - easingT) * destinationRotationYTable[static_cast<uint32_t>(turnFirstRotationY_)] + easingT * destinationRotationY;
	}
}

void Player::MapCollisionDecision(CollisionMapInfo& info) {
	// 上方向
	MapCollisionDecisionTop(info);
	// 下方向
	MapCollisionDecisionBottom(info);
	// 右方向
	MapCollisionDecisionRight(info);
	// 左方向
	MapCollisionDecisionLeft(info);
}

void Player::MapCollisionDecisionTop(CollisionMapInfo& info) {
	// 上昇中か
	if (info.movement.y <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	Vector3 center;
	center.x = worldTransform_.translation_.x + info.movement.x;
	center.y = worldTransform_.translation_.y + info.movement.y;
	center.z = worldTransform_.translation_.z + info.movement.z;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(center, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒットしているか
	if (hit) {
		// 移動後の上端座標
		Vector3 movementposition;
		movementposition.x = center.x;
		movementposition.y = center.y + kHeight / 2.0f;
		movementposition.z = center.z;

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(movementposition);

		Vector3 preMovementposition;
		preMovementposition.x = worldTransform_.translation_.x;
		preMovementposition.y = worldTransform_.translation_.y + kHeight / 2.0f;
		preMovementposition.z = worldTransform_.translation_.z;

		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(preMovementposition);
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			// めり込みを排除する方向に移動量を設定する
			float yMovement = (rect.bottom - worldTransform_.translation_.y) - (kHeight / 2.0f + kBlank);
			info.movement.y = std::max(0.0f, yMovement);
			// 天井にあたったことを記録する
			info.ceilingHit = true;
		}
	}
}

void Player::MapCollisionDecisionBottom(CollisionMapInfo& info) {
	// 下降中か
	if (info.movement.y >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	Vector3 center;
	center.x = worldTransform_.translation_.x + info.movement.x;
	center.y = worldTransform_.translation_.y + info.movement.y;
	center.z = worldTransform_.translation_.z + info.movement.z;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(center, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真下の当たり判定を行う
	bool hit = false;

	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒットしているか
	if (hit) {
		// 移動後の下端座標
		Vector3 movementposition;
		movementposition.x = center.x;
		movementposition.y = center.y - kHeight / 2.0f;
		movementposition.z = center.z;

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(movementposition);

		Vector3 preMovementposition;
		preMovementposition.x = worldTransform_.translation_.x;
		preMovementposition.y = worldTransform_.translation_.y - kHeight / 2.0f;
		preMovementposition.z = worldTransform_.translation_.z;

		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(preMovementposition);
		if (indexSetNow.yIndex != indexSet.yIndex) {

			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			// めり込みを排除する方向に移動量を設定する
			float yMovement = (rect.top - worldTransform_.translation_.y) + (kHeight / 2.0f + kBlank);
			info.movement.y = std::max(0.0f, yMovement);
			// 床にあたったことを記録する
			info.landing = true;
		}
	}
}

void Player::MapCollisionDecisionRight(CollisionMapInfo& info) {
	// 右移動しているか
	if (velocity_.x <= 0.0f) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	Vector3 center;
	center.x = worldTransform_.translation_.x + info.movement.x;
	center.y = worldTransform_.translation_.y + info.movement.y;
	center.z = worldTransform_.translation_.z + info.movement.z;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(center, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 右の当たり判定を行う
	bool hit = false;

	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにあたったか
	if (hit) {
		// 移動後の右端座標
		Vector3 movementposition;
		movementposition.x = center.x + kWidth / 2.0f;
		movementposition.y = center.y;
		movementposition.z = center.z;

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(movementposition);

		Vector3 preMovementposition;
		preMovementposition.x = worldTransform_.translation_.x + kWidth / 2.0f;
		preMovementposition.y = worldTransform_.translation_.y;
		preMovementposition.z = worldTransform_.translation_.z;

		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(preMovementposition);
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			// めり込みを排除する方向に移動量を設定する
			float xMovement = (rect.right - worldTransform_.translation_.x) - (kWidth * 2.0f + kBlank);
			info.movement.x = std::max(0.0f, xMovement);
			// 壁にあたったことを記録する
			info.wallTouch = true;
		}
	}
}

void Player::MapCollisionDecisionLeft(CollisionMapInfo& info) {
	// 左移動しているか
	if (velocity_.x >= 0.0f) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	Vector3 center;
	center.x = worldTransform_.translation_.x + info.movement.x;
	center.y = worldTransform_.translation_.y + info.movement.y;
	center.z = worldTransform_.translation_.z + info.movement.z;

	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(center, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 左の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにあたったか
	if (hit) {
		// 移動後の左端座標
		Vector3 movementposition;
		movementposition.x = center.x - kWidth / 2.0f;
		movementposition.y = center.y;
		movementposition.z = center.z;

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(movementposition);

		Vector3 preMovementposition;
		preMovementposition.x = worldTransform_.translation_.x - kWidth / 2.0f;
		preMovementposition.y = worldTransform_.translation_.y;
		preMovementposition.z = worldTransform_.translation_.z;

		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(preMovementposition);
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

			// めり込みを排除する方向に移動量を設定する
			float xMovement = (rect.left - worldTransform_.translation_.x) + (kWidth / 2.0f + kBlank);
			info.movement.x = std::max(0.0f, xMovement);
			// 壁にあたったことを記録する
			info.wallTouch = true;
		}
	}
}

void Player::JudgmentResultMovement(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_.x += info.movement.x;
	worldTransform_.translation_.y += info.movement.y;
	worldTransform_.translation_.z += info.movement.z;
}

void Player::CeilingHitProcess(const CollisionMapInfo& info) {
	// 天井にあたった
	if (info.ceilingHit) {
		velocity_.y = 0;
	}
}

void Player::WallTouch(const CollisionMapInfo& info) {
	// 壁接触による減速
	if (info.wallTouch) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

void Player::GroundSwitching(const CollisionMapInfo& info) {
	// 自キャラが接地状態か
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			// 移動後の4つの角の座標
			std::array<Vector3, kNumCorner> positionNew;

			Vector3 center;
			center.x = worldTransform_.translation_.x + info.movement.x;
			center.y = worldTransform_.translation_.y + info.movement.y;
			center.z = worldTransform_.translation_.z + info.movement.z;

			for (uint32_t i = 0; i < positionNew.size(); ++i) {
				positionNew[i] = CornerPosition(center, static_cast<Corner>(i));
			}

			MapChipType mapChipType;
			// 真下の当たり判定を行う
			bool hit = false;

			// 左下点の判定
			positionNew[kLeftBottom].y += kBlankBottom;

			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 右下点の判定
			positionNew[kRightBottom].y += kBlankBottom;

			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 落下開始
			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	} else {
		// 着地フラグ
		if (info.landing) {
			// 着地状態に切り替える(落下を止める)
			onGround_ = true;
			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		}
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 result;

	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0},
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}
    };

	result.x = center.x + offsetTable[static_cast<uint32_t>(corner)].x;
	result.y = center.y + offsetTable[static_cast<uint32_t>(corner)].y;
	result.z = center.z + offsetTable[static_cast<uint32_t>(corner)].z;

	return result;
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABBClass::AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABBClass::AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {
	if (IsAttack()) {
		return;
	}

	(void)enemy;
	isDead_ = true;
}

void Player::OnCollision(const ShieldEnemy* enemy) {
	if (IsAttack() || behavior_ == Behavior::kKnockback) {
		return;
	}

	(void)enemy;
	isDead_ = true;
}

void Player::BehaviorRootUpdate(CollisionMapInfo& info) {

	// 攻撃キーを押したら攻撃ビヘイビアをリクエスト
	if (Input::GetInstance()->PushKey(DIK_A)) {
		behaviorRequest_ = Behavior::kAttack;
	}

	// 移動入力
	MoveInput();

	// 移動量に速度の値をコピー
	info.movement.x = velocity_.x;
	info.movement.y = velocity_.y;
	info.movement.z = velocity_.z;

	// 旋回制御
	TurningControl();
}

void Player::BehaviorRootInitialize() {}

void Player::BehaviorAttackUpdate(CollisionMapInfo& info) {
	// 予備動作
	attackParameter_++;

	float t = 0;

	// 攻撃動作用の速度
	Vector3 velocity{};

	// 攻撃フェーズ(サブフェーズ)の更新
	switch (attackPhase_) {
	case AttackPhase::kReservoir:
	default:

		t = static_cast<float>(attackParameter_) / 3.0f;

		worldTransform_.scale_.z = Easing::Lerp(1.0f, 0.3f, Easing::OutQuad(t));
		worldTransform_.scale_.y = Easing::Lerp(1.0f, 1.6f, Easing::OutQuad(t));

		// 前進動作へ移行
		if (static_cast<float>(attackParameter_) >= 3.0f) {
			attackPhase_ = AttackPhase::kRush;

			// カウンターをリセット
			attackParameter_ = 0;
		}
		break;
	case AttackPhase::kRush:

		if (lrDirection_ == LRDirection::kRight) {
			velocity = attackVelocity_;
		} else {
			velocity.x = -attackVelocity_.x;
			velocity.y = -attackVelocity_.y;
			velocity.z = -attackVelocity_.z;
		}

		t = static_cast<float>(attackParameter_) / 5.0f;

		worldTransform_.scale_.z = Easing::Lerp(0.3f, 1.3f, Easing::OutQuad(t));
		worldTransform_.scale_.y = Easing::Lerp(1.6f, 0.7f, Easing::InQuad(t));

		// 余韻動作へ移行
		if (static_cast<float>(attackParameter_) >= 5.0f) {
			attackPhase_ = AttackPhase::kReverberation;

			// カウンターをリセット
			attackParameter_ = 0;
		}

		break;
	case AttackPhase::kReverberation:

		t = static_cast<float>(attackParameter_) / 2.0f;

		worldTransform_.scale_.z = Easing::Lerp(1.3f, 1.0f, Easing::OutQuad(t));
		worldTransform_.scale_.y = Easing::Lerp(0.7f, 1.0f, Easing::OutQuad(t));

		// 通常状態に戻す
		if (static_cast<float>(attackParameter_) >= 2.0f) {
			behaviorRequest_ = Behavior::kRoot;
		}
		break;
	}

	// 衝突情報を初期化
	info.movement = velocity;
}

void Player::BehaviorAttackInitialize() {
	// カウンター初期化
	attackParameter_ = 0;
	attackPhase_ = AttackPhase::kReservoir;

	velocity_ = {0, 0, 0};
}

void Player::BehaviorKnockbackUpdate(CollisionMapInfo& info) {
	// カウント増加
	knockbackParameter_++;

	// イージング用
	float t = 0.0f;

	// フェーズの更新
	switch (knockbackPhase_) {
	case Player::KnockbackPhase::kBlowAway:
		t = static_cast<float>(knockbackParameter_) / 6.0f;

		if (lrDirection_ == LRDirection::kRight) {
			velocity_.x = Easing::Lerp(0.0f, -1.0f, Easing::InQuad(t)) * 2;
		} else {
			velocity_.x = Easing::Lerp(0.0f, 1.0f, Easing::InQuad(t)) * 2;
		}

		if (static_cast<float>(knockbackParameter_) >= 6.0f) {
			knockbackPhase_ = KnockbackPhase::kRebuild;
			knockbackParameter_ = 0;
			velocity_.x = 0.0f;
		}

		break;
	case Player::KnockbackPhase::kRebuild:

		if (knockbackParameter_ >= 30.0f) {
			// 通常状態に戻す
			behaviorRequest_ = Behavior::kRoot;
		}

		break;
	}

	// 移動量に速度の値をコピー
	info.movement = velocity_;
}

void Player::BehaviorKnockbackInitialize() {
	// カウントリセット
	knockbackParameter_ = 0;

	knockbackPhase_ = KnockbackPhase::kBlowAway;

	velocity_ = {0, 0, 0};
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
}

bool Player::IsAttack() {
	switch (behavior_) {
	case Behavior::kUnknown:
	case Behavior::kRoot:
	case Behavior::kKnockback:
		return false;
	case Behavior::kAttack:
		return true;
	}

	return false;
}
