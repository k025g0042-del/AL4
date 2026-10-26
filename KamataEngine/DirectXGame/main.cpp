#include <Windows.h>
#include "GameManager.h"
#include "KamataEngine.h"

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	
	// エンジンの初期化
	KamataEngine::Initialize(L"LE2B_18_ニシカワ_ナナミ_殲滅タックル");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommmon = DirectXCommon::GetInstance();

	// ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	GameManager* gameManager = new GameManager;
	gameManager->Initialize();

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

		gameManager->Updade();

		// ImGui受付終了
		imguiManager->End();

		/*
		    描画処理
		————————————————————————————————————————————————————————————————————————————————————————*/
		// 描画開始
		dxCommmon->PreDraw();

		gameManager->Draw();

		// 軸表示の描画
		AxisIndicator::GetInstance()->Draw();

		// ImGui描画
		imguiManager->Draw();

		// 描画終了
		dxCommmon->PostDraw();
	}

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}
