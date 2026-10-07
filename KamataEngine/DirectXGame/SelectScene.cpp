#include "SelectScene.h"

using namespace KamataEngine;

void SelectScene::Initialize(StageManager* stageDataManager) {
	// モデルインスタンスの作成
	arrow_ = Model::CreateFromOBJ("arrow", true);

	modelSkydome_ = Model::CreateFromOBJ("celestialSphere", true);

	selectText_ = Model::CreateFromOBJ("SelectScene", true);

	// 天球の作成
	skydome_ = new Skydome();

	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	// 引数をメンバ変数に記録する
	stageManager_ = stageDataManager;

	// カメラの初期化
	camera_.farZ = 1080.0f;
	camera_.Initialize();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {-15, 11, -5};
	worldTransform_.rotation_ = {0, 3.14f, 0};
	WorldTransformUpdate(worldTransform_);

	worldTransformText_.Initialize();
	worldTransformText_.rotation_ = {0, 3.14f, 0};
	worldTransformText_.scale_ = {15, 15, 1};
	WorldTransformUpdate(worldTransformText_);

	// フェードの初期化
	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, 3);
}

void SelectScene::Update() {
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
			switch (select_) {
			case SelectScene::Select::kTitleScene:
				break;
			case SelectScene::Select::kStage1:
			case SelectScene::Select::kStage2:
				stageManager_->SetCurrentStageIndex(int(select_) - 1);
				break;
			default:
				break;
			}
			fade_->Start(Fade::Status::FadeOut, 3);
			phase_ = Phase::kFadeOut;
		}

		if (Input::GetInstance()->TriggerKey(DIK_UP)) {
			switch (select_) {
			case SelectScene::Select::kTitleScene:
				select_ = Select::kStage2;
				worldTransform_.translation_ = {-15, 0, -5};
				break;
			case SelectScene::Select::kStage1:
				break;
			case SelectScene::Select::kStage2:
				select_ = Select::kStage1;
				worldTransform_.translation_ = {-15, 11, -5};
				break;
			default:
				break;
			}
		}

		if (Input::GetInstance()->TriggerKey(DIK_DOWN)) {
			switch (select_) {
			case SelectScene::Select::kTitleScene:
				break;
			case SelectScene::Select::kStage1:
				select_ = Select::kStage2;
				worldTransform_.translation_ = {-15, 0, -5};

				break;
			case SelectScene::Select::kStage2:
				select_ = Select::kTitleScene;
				worldTransform_.translation_ = {-15, -10, -5};

				break;
			default:
				break;
			}
		}

		break;
	case Phase::kFadeOut:
		fade_->Update();

		if (fade_->IsFinished()) {
			finished_ = true;
		}

		break;
	}

	worldTransform_.rotation_.x += 0.01f;
	WorldTransformUpdate(worldTransform_);
}

void SelectScene::Draw() {

	Model::PreDraw();

	// 天球の描画
	skydome_->Draw();

	selectText_->Draw(worldTransformText_, camera_);

	arrow_->Draw(worldTransform_, camera_);

	Model::PostDraw();

	fade_->Draw();
}

SelectScene::~SelectScene() {
	delete fade_;
	delete arrow_;
	delete selectText_;
}
