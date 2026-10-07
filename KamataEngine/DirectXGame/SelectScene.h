#pragma once

#include "Fade.h"
#include "KamataEngine.h"
#include "Skydome.h"
#include "StageManager.h"
#include "WorldTransformUpDate.h"
#include <numbers>


class SelectScene {
public:
	/// <summary>
	/// 選択している場所のイーナム
	/// </summary>
	enum class Select {
		kTitleScene,
		kStage1,
		kStage2,
	};

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

	// カメラ
	KamataEngine::Camera camera_;

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform worldTransformText_;

	// 終了フラグ
	bool finished_ = false;

	// フェード
	Fade* fade_ = nullptr;

	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	// ステージマネージャー参照用のポインタ
	StageManager* stageManager_ = nullptr;

	// 選択している場所
	Select select_ = Select::kStage1;

	// モデル
	KamataEngine::Model* arrow_ = nullptr;

	KamataEngine::Model* modelSkydome_ = nullptr;

	KamataEngine::Model* selectText_ = nullptr;

	// 天球
	Skydome* skydome_ = nullptr;

public:
	void Initialize(StageManager* stageDataManager);

	void Update();

	void Draw();

	~SelectScene();

	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

	StageManager* GetStageManager() { return stageManager_; }

	Select GetSelect() { return select_; }
};
