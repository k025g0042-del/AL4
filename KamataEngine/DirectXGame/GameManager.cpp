#include "GameManager.h"

void GameManager::Initialize() {
	// クラスインスタンスの生成
	stageManager = new StageManager;

	// ステージデータファイルを読み込む
	stageManager->StageDataLoading();

#ifdef _DEBUG
	// デバック設定ファイル読み込み
	LoadDebugSettings();

	// 最初のシーンの初期化
	scene_ = Scene::kGame;

	gameScene = new GameScene;
	gameScene->Initialize(stageManager);

#else
	// 最初のシーンの初期化
	scene_ = Scene::kTitle;

	titleScene = new TitleScene;
	titleScene->Initialize();

#endif //  _DEBUG
}

void GameManager::Updade() {
	// シーン切り替え
	ChangeScene();

	// 現在シーン更新
	UpdateScene();
}

void GameManager::Draw() {
	// 現在シーンの描画
	DrawScene();
}

GameManager::~GameManager() {
	// ゲームシーンの解放
	delete gameScene;
	// nullptrの代入
	gameScene = nullptr;

	delete titleScene;
	titleScene = nullptr;

	// クラスインスタンスの開放
	delete stageManager;
}

void GameManager::ChangeScene() {
	switch (scene_) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			// シーン変更
			scene_ = Scene::kSelect;

			// 旧シーンの解放
			delete titleScene;
			titleScene = nullptr;

			// 新シーンの生成と初期化
			selectScene = new SelectScene;
			selectScene->Initialize(stageManager);
		}

		break;
	case Scene::kSelect:
		if (selectScene->IsFinished()) {
			switch (selectScene->GetSelect()) {
			case SelectScene::Select::kTitleScene:
				scene_ = Scene::kTitle;

				// 旧シーンの解放
				delete selectScene;
				selectScene = nullptr;

				// 新シーンの生成と初期化
				titleScene = new TitleScene;
				titleScene->Initialize();

				break;
			case SelectScene::Select::kStage1:
			case SelectScene::Select::kStage2:
				// シーン変更
				scene_ = Scene::kGame;

				stageManager->SetCurrentStageIndex(selectScene->GetStageManager()->GetCurrentStageIndex());

				// 旧シーンの解放
				delete selectScene;
				selectScene = nullptr;

				// 新シーンの生成と初期化
				gameScene = new GameScene;
				gameScene->Initialize(stageManager);
				break;
			default:
				break;
			}
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			// シーン変更
			scene_ = Scene::kTitle;

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

void GameManager::UpdateScene() {
	switch (scene_) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kSelect:
		selectScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}
}

void GameManager::DrawScene() {
	switch (scene_) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kSelect:
		selectScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
}

void GameManager::LoadDebugSettings() {
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
