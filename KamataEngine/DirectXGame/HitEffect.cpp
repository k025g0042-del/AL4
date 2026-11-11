#include "HitEffect.h"
#include "WorldTransformUpDate.h"
#include <random>
#define _USE_MATH_DEFINES
#include "Easing.h"
#include <math.h>

using namespace KamataEngine;

// 静的メンバ変数の実体
Model* HitEffect::model_ = nullptr;
Camera* HitEffect::camera_ = nullptr;

void HitEffect::Initialize(const Vector3& occurrencePosition) {
	circleWorldTransform_.Initialize();

	model_->SetAlpha(1.0f);

	// 円形エフェクト
	circleWorldTransform_.translation_ = occurrencePosition;
	circleWorldTransform_.translation_.z -= 0.5f;

	// 乱数生成エンジン
	std::random_device seedGenerator;

	// メルセンヌ・ツイスターエンジン(64bit版)
	std::mt19937_64 randomEngine;

	// メルセンヌ・ツイスターエンジンの初期化
	randomEngine.seed(seedGenerator());

	// 楕円エフェクト
	for (WorldTransform& worldTransform : ellipseWorldTransform_) {
		// 指定範囲の乱数生成器(浮動小数点用)
		std::uniform_real_distribution<float> rotationDistrbuton(0.0f, 2.0f * float(M_PI));

		// 乱数の取得
		random_ = rotationDistrbuton(randomEngine);

		worldTransform.scale_ = {0.1f, 1.5f, 1.0f};
		worldTransform.rotation_ = {0.0f, 0.0f, random_};
		worldTransform.translation_ = occurrencePosition;
		worldTransform.translation_.z -= 0.5f;

		worldTransform.Initialize();
	}
}

void HitEffect::Update() {

	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_;

		switch (behavior_) {
		case HitEffect::kSpread:
		case HitEffect::kFeadOut:
			timer_ = 0.0f;
			break;
		case HitEffect::kLapse:
			break;
		}

		// 振る舞いリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case HitEffect::kSpread:
		timer_ += 1.0f / 30.0f;

		circleWorldTransform_.scale_.x = Easing::Lerp(0.0f, 1.0f, Easing::InOutQuad(timer_));
		circleWorldTransform_.scale_.y = Easing::Lerp(0.0f, 1.0f, Easing::InOutQuad(timer_));
		circleWorldTransform_.scale_.z = Easing::Lerp(0.0f, 1.0f, Easing::InOutQuad(timer_));

		for (WorldTransform& worldTransform : ellipseWorldTransform_) {
			worldTransform.scale_.x = Easing::Lerp(0.0f, 0.1f, Easing::InOutQuad(timer_));
			worldTransform.scale_.y = Easing::Lerp(0.0f, 1.5f, Easing::InOutQuad(timer_));
			worldTransform.scale_.z = Easing::Lerp(0.0f, 1.0f, Easing::InOutQuad(timer_));
		}

		if (timer_ >= 1.0f) {
			behaviorRequest_ = Behavior::kFeadOut;
		}

		break;
	case HitEffect::kFeadOut:
		timer_ += 1.0f / 60.0f;

		model_->SetAlpha(Easing::Lerp(1.0f, 0.0f, Easing::InQuad(timer_)));

		if (timer_ >= 1.0f) {
			behaviorRequest_ = Behavior::kLapse;
		}

		break;
	case HitEffect::kLapse:

		if (!isDead_) {
			isDead_ = true;
		}

		break;
	}

	// 円形エフェクト
	WorldTransformUpdate(circleWorldTransform_);

	for (WorldTransform& worldTransform : ellipseWorldTransform_) {
		WorldTransformUpdate(worldTransform);
	}
}

void HitEffect::Draw() {
	for (WorldTransform& worldTransform : ellipseWorldTransform_) {
		model_->Draw(worldTransform, *camera_);
	}

	// 円形エフェクト
	model_->Draw(circleWorldTransform_, *camera_);
}

HitEffect* HitEffect::Create(const Vector3& occurrencePosition) {
	// インスタンス作成
	HitEffect* instance = new HitEffect();

	// newの失敗を検出
#ifdef _DEBUG
	assert(instance);
#endif // _DEBUG

	// インスタンスの初期化
	instance->Initialize(occurrencePosition);

	// 初期化したインスタンスを返す
	return instance;
}
