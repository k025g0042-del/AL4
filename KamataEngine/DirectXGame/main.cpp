#include <Windows.h>
#include "GameScene.h"
#include "KamataEngine.h"
#include "StageManager.h"
#include "TitleScene.h"

using namespace KamataEngine;

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;
StageManager* stageManager = nullptr;

// シーン(型)
enum class Scene {
	kUnknown = 0,

	kTitle,
	kGame,
};

// 現在シーン(型)
Scene scene = Scene::kUnknown;

// シーンの切り替え処理
void ChangeScene();

// シーンの更新
void UpdateScene();

// シーンの描画
void DrawScene();

// 設定ファイルの読み込み
void LoadDebugSettings();

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"LC1A_19_ニシカワ_ナナミ_AL3");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommmon = DirectXCommon::GetInstance();

	// ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// クラスインスタンスの生成
	stageManager = new StageManager;

	// ステージデータファイルを読み込む
	stageManager->StageDataLoading();

#ifdef _DEBUG
	// デバック設定ファイル読み込み
	LoadDebugSettings();

	// 最初のシーンの初期化
	scene = Scene::kGame;

	gameScene = new GameScene;
	gameScene->Initialize(stageManager);

#else
	// 最初のシーンの初期化
	scene = Scene::kTitle;

	titleScene = new TitleScene;
	titleScene->Initialize();

#endif //  _DEBUG

	// メインループ
	while (true) {

		/*
		    更新処理
		————————————————————————————————————————————————————————————————————————————————————————*/

		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();

		// シーン切り替え
		ChangeScene();

		// 現在シーン更新
		UpdateScene();

		// ImGui受付終了
		imguiManager->End();

		/*
		    描画処理
		————————————————————————————————————————————————————————————————————————————————————————*/
		// 描画開始
		dxCommmon->PreDraw();

		// 現在シーンの描画
		DrawScene();

		// 軸表示の描画
		AxisIndicator::GetInstance()->Draw();

		// ImGui描画
		imguiManager->Draw();

		// 描画終了
		dxCommmon->PostDraw();
	}

	// ゲームシーンの解放
	delete gameScene;
	// nullptrの代入
	gameScene = nullptr;

	delete titleScene;
	titleScene = nullptr;

	// クラスインスタンスの開放
	delete stageManager;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGame;

			// 旧シーンの解放
			delete titleScene;
			titleScene = nullptr;

			// 新シーンの生成と初期化
			gameScene = new GameScene;
			gameScene->Initialize(stageManager);
		}

		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;

			// 旧シーンの解放
			delete gameScene;
			gameScene = nullptr;

			// 新シーンの生成と初期化
			titleScene = new TitleScene;
			titleScene->Initialize();
		} else if (gameScene->GetReloadRquested()) {
			// シーンリロード
			delete gameScene;
			gameScene = nullptr;
			gameScene = new GameScene;
			gameScene->Initialize(stageManager);
		}

		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
}

void LoadDebugSettings() {
	// ファイルパス
	const std::string filePath = "DebugSettings.ini";

	// isstreamでステージデータファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open() && "ステージデータファイルが存在しません");

	// ファイルの内容を格納するstringstreamの宣言
	std::stringstream stringstream;

	// ファイルの内容をstringstreamにコピーする
	stringstream << file.rdbuf();

	// ファイルを閉じる
	file.close();

	// １行分の内容を格納するstringの宣言
	std::string lineString;
	std::getline(stringstream, lineString);

	// 1行分の内容を格納するstringstreamを宣言して、stringから変換
	std::istringstream lineStream(lineString);

	// スペース区切りの一つ分を格納するstringの宣言
	std::string word;

	// スペース区切りで次のデータを取得する
	std::getline(lineStream, word, ' ');

	// ステージ設定
	if (word == "InitialStage") {
		// スペース区切りで次のデータを取得する
		std::getline(lineStream, word, ' ');
		stageManager->SetCurrentStageIndexByName(word);
	}
}
