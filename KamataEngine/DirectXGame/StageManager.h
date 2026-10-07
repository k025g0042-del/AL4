#pragma once
#include <cassert>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

struct StageData {
	std::string name;  // ステージ名(フィールドCSVファイル名)
	int32_t speed; // スピード
};

/// <summary>
/// ステージ管理
/// </summary>
class StageManager {
private:
	// 全ステージデータ
	std::vector<StageData> stageDatas_;

	// 現在のステージ番号
	int32_t currentStageIndex_ = 0;

public:
	/// <summary>
	/// ステージデータファイルの読み込み
	/// </summary>
	void StageDataLoading();

	/// <summary>
	/// ステージデータの取得
	/// </summary>
	/// <param name="index">ステージ番号</param>
	/// <returns>ステージデータ</returns>
	const StageData& GetStageData(uint32_t index) const {
		assert(index <= stageDatas_.size());
		return stageDatas_[index];
	}

	void SetCurrentStageIndex(int32_t index) {
		assert(index <= stageDatas_.size());
		currentStageIndex_ = index;
	}

	int32_t GetCurrentStageIndex() const { return currentStageIndex_; }

	/// <summary>
	/// 現在ステージのステージデータ取得
	/// </summary>
	///  <param name="index">ステージ番号</param>
	/// <returns>ステージデータ</returns>
	const StageData& GetCurrentStageData() const { return GetStageData(currentStageIndex_); }

	/// <summary>
	/// ステージ名指定で現在ステージ番号設定
	/// </summary>
	/// <param name="name">ステージ名</param>
	void SetCurrentStageIndexByName(const std::string& name);
};
