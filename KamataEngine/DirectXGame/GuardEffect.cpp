#include "GuardEffect.h"
#include "WorldTransformUpDate.h"
#include <random>
#define _USE_MATH_DEFINES
#include "Easing.h"
#include <math.h>

using namespace KamataEngine;

// 静的メンバ変数の実体
Model* GuardEffect::model_ = nullptr;
Camera* GuardEffect::camera_ = nullptr;

void GuardEffect::Initialize(const Vector3& occurrencePosition) {
	circleWorldTransform_.Initialize();

	model_->SetAlpha(1.0f);

	// 円形エフェクト
	circleWorldTransform_.translation_ = occurrencePosition;
	circleWorldTransform_.translation_.z -= 0.5f;
}

void GuardEffect::Update() {

	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_;

		switch (behavior_) {
		case GuardEffect::kSpread:
		case GuardEffect::kFeadOut:
			timer_ = 0.0f;
			break;
		case GuardEffect::kLapse:
			break;
		}

		// 振る舞いリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case GuardEffect::kSpread:
		timer_ += 1.0f / 30.0f;

		circleWorldTransform_.scale_.x = Easing::Lerp(0.0f, 2.0f, Easing::InOutQuad(timer_));
		circleWorldTransform_.scale_.y = Easing::Lerp(0.0f, 2.0f, Easing::InOutQuad(timer_));
		circleWorldTransform_.scale_.z = Easing::Lerp(0.0f, 2.0f, Easing::InOutQuad(timer_));

		if (timer_ >= 1.0f) {
			behaviorRequest_ = Behavior::kFeadOut;
		}

		break;
	case GuardEffect::kFeadOut:
		timer_ += 1.0f / 60.0f;

		model_->SetAlpha(Easing::Lerp(1.0f, 0.0f, Easing::InQuad(timer_)));

		if (timer_ >= 1.0f) {
			behaviorRequest_ = Behavior::kLapse;
		}

		break;
	case GuardEffect::kLapse:

		if (!isDead_) {
			isDead_ = true;
		}

		break;
	}

	// 円形エフェクト
	WorldTransformUpdate(circleWorldTransform_);
}

void GuardEffect::Draw() {
	// 円形エフェクト
	model_->Draw(circleWorldTransform_, *camera_);
}

GuardEffect* GuardEffect::Create(const Vector3& occurrencePosition) {
	// インスタンス作成
	GuardEffect* instance = new GuardEffect();

	// newの失敗を検出
#ifdef _DEBUG
	assert(instance);
#endif // _DEBUG

	// インスタンスの初期化
	instance->Initialize(occurrencePosition);

	// 初期化したインスタンスを返す
	return instance;
}
