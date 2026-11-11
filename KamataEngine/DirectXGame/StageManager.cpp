#include "StageManager.h"



void StageManager::StageDataLoading() {
	// ステージデータファイルのパス
	const std::string filePath = "Resources/fields/stageDatas.csv";

	//isstreamでステージデータファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open() && "ステージデータファイルが存在しません");

	//ファイルの内容を格納するstringstreamの宣言
	std::stringstream stringstream;

	//ファイルの内容をstringstreamにコピーする
	stringstream << file.rdbuf();

	// ファイルを閉じる
	file.close();

	//ステージデータを最終行まで1行ずつ読み込む
	//１行分の内容を格納するstringの宣言
	std::string lineString;
	while (std::getline(stringstream,lineString)) {
		//1行分の内容を格納するstringstreamを宣言して、stringから変換
		std::istringstream lineStream(lineString);

		//ステージデータを格納する構造体
		StageData stageData;

		//カンマ区切りの一つ分を格納するstringの宣言
		std::string word;

		//カンマ区切りで次のデータを取得する
		std::getline(lineStream, word, ',');

		//ステージ名を格納する
		stageData.name = word;

		// カンマ区切りで次のデータを取得する
		std::getline(lineStream, word, ',');

		//整数に変換して制限時間を格納する
		stageData.timeLimit = std::stoi(word);

		//ステージデータテーブルに格納する
		stageDatas_.push_back(stageData);
	}
}

void StageManager::SetCurrentStageIndexByName(const std::string& name) {
	//全ステージデータを検索
	for (size_t i = 0; i < stageDatas_.size(); ++i) {
		//ステージ名が一致したら現在ステージ番号を設定する
		if (stageDatas_[i].name == name) {
			currentStageIndex_ = static_cast<int32_t>(i);

			//目的を達したので関数を抜ける
			return;
		}
	}
	assert(false && "指定されたステージ名は存在しません");
}
