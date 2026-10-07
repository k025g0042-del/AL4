#pragma once

#include "GameScene.h"
#include "SelectScene.h"
#include "StageManager.h"
#include "TitleScene.h"

class GameManager {
private:
	GameScene* gameScene = nullptr;
	TitleScene* titleScene = nullptr;
	SelectScene* selectScene = nullptr;
	StageManager* stageManager = nullptr;

	// シーン(型)
	enum class Scene {
		kUnknown = 0,

		kTitle,
		kSelect,
		kGame,
	};

	// 現在シーン(型)
	Scene scene_ = Scene::kUnknown;

public:
	void Initialize();

	void Updade();

	void Draw();

	// シーンの切り替え処理
	void ChangeScene();

	// シーンの更新
	void UpdateScene();

	// シーンの描画
	void DrawScene();

	// 設定ファイルの読み込み
	void LoadDebugSettings();

	~GameManager();
};
