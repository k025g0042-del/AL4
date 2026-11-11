#pragma once
#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "Fade.h"
#include "HitEffect.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include <vector>

class StageManager;

// ゲームシーン
class GameScene {
private:
	// テクスチャハンドル
	// uint32_t textureHandle_ = 0;

	// 3Dモデル
	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Model* deathParticleModel_ = nullptr;

	KamataEngine::Model* enemyModel_ = nullptr;

	KamataEngine::Model* block_ = nullptr;

	KamataEngine::Model* modelSkydome_ = nullptr;

	KamataEngine::Model* hitEffectModel_ = nullptr;


	// カメラ
	KamataEngine::Camera camera_;

	// 自キャラ
	Player* player_ = nullptr;

	// デスパーティクル
	DeathParticles* deathParticles_ = nullptr;
	bool isDeathParticleBeing_ = true;

	// 敵キャラ
	std::list<Enemy*> enemies_;

	// エフェクト
	std::list<HitEffect*> hitEffects_;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	// デバックカメラ有効
	bool isDebugCameraActive_ = false;

	// デバックカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	// 天球
	Skydome* skydome_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	// 追従カメラ
	CameraController* cameraFollow_ = nullptr;

	// ゲームのフェーズ(型)
	/*フェードイン、ゲームプレイ、デス演出、フェードアウト*/
	enum class Phase {
		kFadeIn,
		kPlay,
		kDeath,
		kFadeOut,
	};

	// ゲームの現在フェーズ(変数)
	Phase phase_;

	// 終了フラグ
	bool finished_ = false;

	// フェード
	Fade* fade_ = nullptr;

	// リロード要求フラグ
	bool reloadRequested_ = false;

	//ステージマネージャー参照用のポインタ
	StageManager* stageManager_ = nullptr;

public:
	~GameScene();

	// 初期化
	void Initialize(StageManager* stageDataManager);

	// 更新
	void Update();

	// 描画
	void Draw();

	// フィールドオブジェクトの生成
	void GenerateFieldObjects();

	// エネミー生成
	void GenerateEnemyObjects(const uint8_t& subID, int j, int i);

	/// <summary>
	/// 全ての当たり判定を行う
	/// </summary>
	void CheckAllCollisions();

	// フェーズの切り替え
	void ChangePhase();

	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

	// エフェクトを生成
	void CreateHitEffect(const KamataEngine::Vector3& occurrencePosition);
	void CreateGuardEffect(const KamataEngine::Vector3& occurrencePosition);

	// リロード要求フラグのgetter
	bool GetReloadRquested() const { return reloadRequested_; }
};
