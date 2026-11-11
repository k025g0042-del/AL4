#pragma once
#include "KamataEngine.h"

/// <summary>
/// フェード
/// </summary>
class Fade {
public:
	// フェードの状態
	// 順に、フェードなし、フェードイン中、フェードアウト中
	enum class Status {
		None,
		FadeIn,
		FadeOut,
	};

private:
	KamataEngine::Sprite* sprite_ = nullptr;
	uint32_t textureHandle_ = KamataEngine::TextureManager::Load("fadeBlack.png");

	// 現在のフェードの状態
	Status status_ = Status::None;

	// フェードの持続時間
	float duration_ = 0.0f;

	// 経過時間カウンター
	float counter_ = 0.0f;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// フェード開始
	/// </summary>
	/// <param name="status"></param>
	/// <param name="duration"></param>
	void Start(Status status, float duration);

	/// <summary>
	/// フェード停止
	/// </summary>
	void Stop();

	/// <summary>
	/// フェード終了判定
	/// </summary>
	/// <returns></returns>
	bool IsFinished() const;
};
