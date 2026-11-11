#pragma once
#include "Fade.h"
#include "KamataEngine.h"

class TitleScene {
private:
	/// <summary>
	/// シーンのフェーズ
	/// 順に、フェードイン、メイン部、フェードアウト
	/// </summary>
	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};

	KamataEngine::Model* title_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	float speed_ = 0.05f;

	// 終了フラグ
	bool finished_ = false;

	// フェード
	Fade* fade_ = nullptr;

	//現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

public:
	void Initialize();

	void Update();

	void Draw();

	~TitleScene();

	// デスフラグのgetter
	bool IsFinished() const { return finished_; }
};