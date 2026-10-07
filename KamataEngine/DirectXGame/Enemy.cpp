#include "Enemy.h"
#include "WorldTransformUpDate.h"
#include "cassert"
#include <numbers>
#define _USE_MATH_DEFINES
#include "Easing.h"
#include "MathFunction.h"
#include "Player.h"
#include <math.h>
#include"GameScene.h"


using namespace KamataEngine;

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// NULLポイントチェック
	assert(model);

	model_ = model;

	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.x = std::numbers::pi_v<float> / -2.0f;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 1.0f;

	// 速度を設定する
	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;

	// 行列の更新
	WorldTransformUpdate(worldTransform_);
}

void Enemy::Update() {

	// Behavior遷移
	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_;

		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kWalk:
		default:

			BehaviorWalkInitialize();

			break;
		case Behavior::kDeath:

			BehaviorDeathInitialize();

			break;
		}

		// 振る舞いリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kWalk:

		BehaviorWalkUpdate();

		break;
	case Behavior::kDeath:

		BehaviorDeathUpdate();

		break;
	}

	// 行列の更新
	WorldTransformUpdate(worldTransform_);
}

void Enemy::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}

KamataEngine::Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

AABBClass::AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABBClass::AABB aabb;

	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Enemy::OnCollision(Player* player) {
	if (behavior_ == Behavior::kDeath) {
		// 敵が既にやられているなら何もしない
		return;
	}

	// プレイヤーが攻撃中なら敵が死ぬ
	if (player->IsAttack()) {
		// 敵の振る舞いをデス演出に変更
		behaviorRequest_ = Behavior::kDeath;

		//敵と自キャラの中間位置にエフェクトを生成
		Vector3 effectPos;
		effectPos.x = (worldTransform_.translation_.x + player->GetWorldTransformX()) / 2.0f;
		effectPos.y = (worldTransform_.translation_.y + player->GetWorldTransformY()) / 2.0f;
		effectPos.z = (worldTransform_.translation_.z + player->GetWorldTransformZ()) / 2.0f;
		gameScene_->CreateHitEffect(effectPos);
	}
}
void Enemy::BehaviorWalkUpdate() {
	// タイマーを加算
	walkTimer_ += 1.0f / 60.0f;

	// 回転アニメーション
	float param = std::sin(static_cast<float>(M_PI * 2) * (walkTimer_ / kWalkMotionTime));
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.z = MathFunction::DegreeToRadian(degree);

	// 移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
}

void Enemy::BehaviorWalkInitialize() {
	worldTransform_.rotation_.x = std::numbers::pi_v<float> / -2.0f;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 1.0f;

	// 速度を設定する
	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;

	// 行列の更新
	WorldTransformUpdate(worldTransform_);
}

void Enemy::BehaviorDeathUpdate() {
	animationTimer_ += 1.0f / 120.0f;

	worldTransform_.rotation_.y = Easing::Lerp(0.0f, 1.5f, Easing::OutQuad(animationTimer_));
	worldTransform_.rotation_.x = Easing::Lerp(0.0f, -1.5f, Easing::OutSine(animationTimer_));
	worldTransform_.rotation_.z = Easing::Lerp(0.0f, 1.5f, Easing::OutSine(animationTimer_));

	worldTransform_.scale_.x = Easing::Lerp(1.0f, 0.5f, Easing::InOutQuad(animationTimer_));
	worldTransform_.scale_.y = Easing::Lerp(1.0f, 0.5f, Easing::InOutQuad(animationTimer_));
	worldTransform_.scale_.z = Easing::Lerp(1.0f, 0.5f, Easing::InOutQuad(animationTimer_));

	if (animationTimer_ >= 1.0f) {
		isDead_ = true;
	}
}

void Enemy::BehaviorDeathInitialize() {

	// コリジョン無効フラグを立てる
	isCollisionDisabled_ = true;

	worldTransform_.rotation_.x = std::numbers::pi_v<float> / -2.0f;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 1.0f;

	animationTimer_ = 0.0f;

	// 行列の更新
	WorldTransformUpdate(worldTransform_);
}
