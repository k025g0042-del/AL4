#define NOMINMAX
#include "CameraController.h"
#include "Player.h"

using namespace KamataEngine;

// 線形補間
Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
	Vector3 result;

	result.x = t * a.x + (1.0f - t) * b.x;
	result.y = t * a.y + (1.0f - t) * b.y;
	result.z = t * a.z + (1.0f - t) * b.z;

	return result;
}

void CameraController::Initialize() {
	//カメラの初期化
	camera_.Initialize();
}

void CameraController::Update() {
	//追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();

	// 追従対象とオフセットと追従対象の速度からカメラの目標座標を計算
	markPosition.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity.x * kVelocityBias;
	markPosition.y = targetWorldTransform.translation_.y + targetOffset_.y + targetVelocity.y * kVelocityBias;
	markPosition.z = targetWorldTransform.translation_.z + targetOffset_.z + targetVelocity.z * kVelocityBias;

	//座標補間によりゆったり追従
	camera_.translation_ = Lerp(camera_.translation_, markPosition, kInterpolationRate);

	//追従対象が画面外に出ないように補正
	camera_.translation_.x = std::max(camera_.translation_.x, targetWorldTransform.translation_.x + margin.left);
	camera_.translation_.x = std::min(camera_.translation_.x, targetWorldTransform.translation_.x + margin.right);
	camera_.translation_.y = std::max(camera_.translation_.y, targetWorldTransform.translation_.y + margin.bottom);
	camera_.translation_.y = std::min(camera_.translation_.y, targetWorldTransform.translation_.y + margin.top);


	//移動範囲制限
	camera_.translation_.x = std::max(camera_.translation_.x, movableArea_.left);
	camera_.translation_.x = std::min(camera_.translation_.x, movableArea_.right);
	camera_.translation_.y = std::max(camera_.translation_.y, movableArea_.bottom);
	camera_.translation_.y = std::min(camera_.translation_.y, movableArea_.top);

	//行列を更新する
	camera_.UpdateMatrix();
}

void CameraController::Reset() {
	//追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	//追従対象とオフセットからカメラの座標を計算
	camera_.translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_.translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
}
