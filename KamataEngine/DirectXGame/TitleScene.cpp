#include "TitleScene.h"
#include "WorldTransformUpDate.h"

using namespace KamataEngine;

void TitleScene::Initialize() {
	title_ = Model::CreateFromOBJ("titleAction", true);
	space_ = Model::CreateFromOBJ("space", true);
	modelSkydome_ = Model::CreateFromOBJ("celestialSphere", true);

	// 天球の作成
	skydome_ = new Skydome();

	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	// カメラの初期化
	camera_.farZ = 1080.0f;
	camera_.Initialize();
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {-20, 0, 0};

	worldTransformSpace_.Initialize();
	worldTransformSpace_.translation_ = {-5, -10, 0};
	WorldTransformUpdate(worldTransformSpace_);

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
	skydome_->Draw();

	title_->Draw(worldTransform_, camera_);

	space_->Draw(worldTransformSpace_, camera_);

	Model::PostDraw();

	fade_->Draw();
}

TitleScene::~TitleScene() { delete fade_; }
