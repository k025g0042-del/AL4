#pragma once
#include "KamataEngine.h"

// マップチップ種別
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
	kPlayer, //プレイヤー
	kEnemy, //エネミー
};

// 1マス分のデータ
struct MapChipDataUnit {
	MapChipType type; // マップチップの種別
	uint8_t subID;    // 種類ごとのサブID
};

// ステージ全体のマップチップデータ
struct MapChipData {
	std::vector<std::vector<MapChipDataUnit>> data;
};

/// <summary>
/// マップチップフィールド
/// </summary>
class MapChipField {
private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 104;

	MapChipData mapChipData_;

public:
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	// 範囲矩形
	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

	// マップチップCSVの文字番号
	enum MapChipCharIndex {
		kChipType = 0,
		kChipSubID = 1,
	};

	// リセット関数
	void ResetMapChipData();

	// 読み込み関数
	void LoadMapChipCsv(const std::string& filePath);

	// マップチップ種別の取得
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	//マップチップサブIDの取得
	uint8_t GetMapChipSubIDByIndex(uint32_t xIndex, uint32_t yIndex);

	// マップチップ座標の取得
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	// 座標からマップチップ番号を計算
	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);

	// ブロックの範囲取得関数
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	// Get関数
	static inline const uint32_t GetNumBlockVirtical() { return kNumBlockVirtical; }
	static inline const uint32_t GetNumBlockHorizontal() { return kNumBlockHorizontal; }
};
