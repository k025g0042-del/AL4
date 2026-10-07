#include "Skydome.h"

using namespace KamataEngine;

void Skydome::Initialize(Model* model, Camera* camera) {
	// NULLポイントチェック
#ifdef _DEBUG
	assert(model);
#endif // _DEBUG

	model_ = model;

	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
}

void Skydome::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Skydome::Draw() {
	// 3Dモデル描画
	model_->Draw(worldTransform_, *camera_);
}
