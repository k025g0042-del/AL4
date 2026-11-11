#include "TitleScene.h"
#include "WorldTransformUpDate.h"

using namespace KamataEngine;

void TitleScene::Initialize() {
	title_ = Model::CreateFromOBJ("titleAction", true);
	// カメラの初期化
	camera_.farZ = 1080.0f;
	camera_.Initialize();
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {-20, 0, 0};

	// フェードの初期化
	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, 3);
}

void TitleScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();

		if (fade_->IsFinished()) {
			fade_->Stop();
			phase_ = Phase::kMain;
		}

		break;
	case Phase::kMain:
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 3);
			phase_ = Phase::kFadeOut;
		}

		break;
	case Phase::kFadeOut:
		fade_->Update();

		if (fade_->IsFinished()) {
			finished_ = true;
		}

		break;
	}

	worldTransform_.translation_.y += speed_;

	if (worldTransform_.translation_.y >= 5.0f) {
		speed_ *= -1;
	} else if (worldTransform_.translation_.y <= 0.0f) {
		speed_ *= -1;
	}

	WorldTransformUpdate(worldTransform_);
}

void TitleScene::Draw() {
	Model::PreDraw();

	title_->Draw(worldTransform_, camera_);

	Model::PostDraw();

	fade_->Draw();
}

TitleScene::~TitleScene() { delete fade_; }
