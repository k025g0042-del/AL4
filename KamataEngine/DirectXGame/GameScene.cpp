#include "GameScene.h"
#include "2d/ImGuiManager.h"
#include "AABBClass.h"
#include "Calculation.h"
#include "StageManager.h"
#include "WorldTransformUpDate.h"

using namespace KamataEngine;

void GameScene::Initialize(StageManager* stageDataManager) {
	// 引数をメンバ変数に記録する
	stageManager_ = stageDataManager;

	// 3Dモデルの生成
	model_ = Model::CreateFromOBJ("playerOnrush", true);

	enemyModel_ = Model::CreateFromOBJ("enemy", true);

	shieldenemyModel_ = Model::CreateFromOBJ("enemyShield", true);

	block_ = Model::CreateFromOBJ("brick", true);

	modelSkydome_ = Model::CreateFromOBJ("celestialSphere", true);

	deathParticleModel_ = Model::CreateFromOBJ("deathParticle", true);

	hitEffectModel_ = Model::CreateFromOBJ("HitEffect", true);
	HitEffect::SetModel(hitEffectModel_);
	HitEffect::SetCamera(&camera_);

	guardEffectModel_ = Model::CreateFromOBJ("GuardEffect", true);
	GuardEffect::SetModel(guardEffectModel_);
	GuardEffect::SetCamera(&camera_);

	clearTextModel_ = Model::CreateFromOBJ("Cleartext", true);

	// カメラの初期化
	camera_.farZ = 1080.0f;
	camera_.Initialize();

	// マップチップフィールドの生成
	mapChipField_ = new MapChipField;

	// 現在のステージデータを取得する
	const StageData& stageData = stageManager_->GetCurrentStageData();

	// ステージファイルパスの生成
	std::string stageFileName = "Resources/fields/" + stageData.name + ".csv ";

	// ステージファイルの読み込み
	mapChipField_->LoadMapChipCsv(stageFileName);

	GenerateFieldObjects();
	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}

			WorldTransformUpdate(*worldTransformBlock);
		}
	}

	// デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	debugCamera_->SetFarZ(1080.0f);

	// 天球の作成
	skydome_ = new Skydome();

	// 天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	// 追従カメラの生成
	cameraFollow_ = new CameraController();

	// 追従カメラの初期化
	cameraFollow_->Initialize();

	// 追従対象をセット
	cameraFollow_->SetTarget(player_);

	// リセット(瞬間合わせ)
	cameraFollow_->Reset();

	cameraFollow_->SetMovableArea({11, 19, 6, 20});
	// ビュープロジェクション行列の転送
	camera_.TransferMatrix();

	// フェードインから開始
	phase_ = Phase::kFadeIn;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 3);
}

void GameScene::Update() {
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		if (isDebugCameraActive_) {
			isDebugCameraActive_ = false;
		} else {
			isDebugCameraActive_ = true;
		}
	}

	ImGui::Begin("window");
	if (ImGui::Button("reload")) {
		reloadRequested_ = true;
	}
	ImGui::End();
#endif

	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();

		if (fade_->IsFinished()) {
			fade_->Stop();
			phase_ = Phase::kPlay;
		}

		if (isDebugCameraActive_) {
			// デバックカメラの更新
			debugCamera_->Update();

			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;

			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {

			// 追従カメラの更新
			cameraFollow_->Update();

			camera_.matView = cameraFollow_->GetCamera().matView;
			camera_.matProjection = cameraFollow_->GetCamera().matProjection;

			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		}

		break;
	case Phase::kPlay:

		if (enemies_.empty() && shieldenemies_.empty()) {
			clearTextWorldTransform_.Initialize();
			clearTextWorldTransform_.translation_ = player_->GetWorldTransform().translation_;
			clearTextWorldTransform_.translation_.y = 7;
			WorldTransformUpdate(clearTextWorldTransform_);
			fade_->Start(Fade::Status::FadeOut, 3);
			phase_ = Phase::kFadeOut;
		}

		// 天球の更新
		skydome_->Update();

		// 自キャラの更新
		player_->Update();

		// エネミーの更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		for (ShieldEnemy* shieldEnemy : shieldenemies_) {
			shieldEnemy->Update();
		}

		// エフェクトの更新
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}

		for (GuardEffect* guardEffect : guardEffects_) {
			guardEffect->Update();
		}

		if (isDebugCameraActive_) {
			// デバックカメラの更新
			debugCamera_->Update();

			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;

			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {

			// 追従カメラの更新
			cameraFollow_->Update();

			camera_.matView = cameraFollow_->GetCamera().matView;
			camera_.matProjection = cameraFollow_->GetCamera().matProjection;

			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}

				WorldTransformUpdate(*worldTransformBlock);
			}
		}

		// 全ての当たり判定を行う
		CheckAllCollisions();

		break;
	case Phase::kDeath:

		// 天球の更新
		skydome_->Update();

		// エネミーの更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		for (ShieldEnemy* shieldEnemy : shieldenemies_) {
			shieldEnemy->Update();
		}

		// エフェクトの更新
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}

		for (GuardEffect* guardEffect : guardEffects_) {
			guardEffect->Update();
		}

		// デスパーティクルの更新
		if (isDeathParticleBeing_) {
			deathParticles_->Update();
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}

				WorldTransformUpdate(*worldTransformBlock);
			}
		}

		if (deathParticles_ && deathParticles_->GetIsFinished()) {
			fade_->Start(Fade::Status::FadeOut, 3);
			phase_ = Phase::kFadeOut;
		}

		break;
	case Phase::kFadeOut:
		fade_->Update();

		if (fade_->IsFinished()) {
			finished_ = true;
		}

		// エネミーの更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		for (ShieldEnemy* shieldEnemy : shieldenemies_) {
			shieldEnemy->Update();
		}

		break;
	}

	// デスフラグの立った敵を削除
	enemies_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	shieldenemies_.remove_if([](ShieldEnemy* shieldEnemy) {
		if (shieldEnemy->IsDead()) {
			delete shieldEnemy;
			return true;
		}
		return false;
	});

	hitEffects_.remove_if([](HitEffect* hiteffect) {
		if (hiteffect->IsDead()) {
			delete hiteffect;
			return true;
		}
		return false;
	});

	guardEffects_.remove_if([](GuardEffect* guardEffect) {
		if (guardEffect->IsDead()) {
			delete guardEffect;
			return true;
		}
		return false;
	});

	ChangePhase();
}

void GameScene::Draw() {
	Model::PreDraw();

	// 天球の描画
	skydome_->Draw();

	switch (phase_) {
	case Phase::kFadeIn:
	case GameScene::Phase::kPlay:
		// 自キャラの描画
		player_->Draw();
		break;
	case GameScene::Phase::kDeath:
		// デスパーティクルの描画
		if (isDeathParticleBeing_) {
			deathParticles_->Draw();
		}
		break;
	case Phase::kFadeOut:
		if (!player_->IsDead()) {
			clearTextModel_->Draw(clearTextWorldTransform_, camera_);
			player_->Draw();
		}

		break;
	}

	// エネミーの描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	for (ShieldEnemy* shieldEnemy : shieldenemies_) {
		shieldEnemy->Draw();
	}

	// ヒットエフェクトの描画
	for (HitEffect* hitEffect : hitEffects_) {
		hitEffect->Draw();
	}

	for (GuardEffect* guardEffect : guardEffects_) {
		guardEffect->Draw();
	}

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}

			block_->Draw(*worldTransformBlock, camera_);
		}
	}

	Model::PostDraw();

	// フェードの描画
	fade_->Draw();
}

void GameScene::GenerateFieldObjects() {
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 一列の要素数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	uint8_t subID;

	// フィールドオブジェクトの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			WorldTransform* worldTransform = new WorldTransform();
			worldTransform->Initialize();
			switch (mapChipField_->GetMapChipTypeByIndex(j, i)) {
			case MapChipType::kBlank:
				break;
			case MapChipType::kBlock:
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
				break;
			case MapChipType::kPlayer:
				// 自キャラの生成
				player_ = new Player();

				// 自キャラの初期化
				player_->Initialize(model_, &camera_, mapChipField_->GetMapChipPositionByIndex(j, i));
				player_->SetMapChipField(mapChipField_);

				// 仮の生成処理
				deathParticles_ = new DeathParticles;
				deathParticles_->Initialize(deathParticleModel_, &camera_, mapChipField_->GetMapChipPositionByIndex(j, i));

				break;
			case MapChipType::kEnemy:
				subID = mapChipField_->GetMapChipSubIDByIndex(j, i);
				GenerateEnemyObjects(subID, j, i);
				break;
			default:
				break;
			}
		}
	}
}

void GameScene::GenerateEnemyObjects(const uint8_t& subID, int j, int i) {
	// 歩く敵の生成
	Enemy* newEnemy = new Enemy();
	// 盾持ちの敵の生成
	ShieldEnemy* newShieldEnemy = new ShieldEnemy();

	switch (subID) {
	case 0:
		// 初期化
		newEnemy->Initialize(enemyModel_, &camera_, mapChipField_->GetMapChipPositionByIndex(j, i));
		newEnemy->SetGameScene(this);
		enemies_.push_back(newEnemy);
		break;
	case 1:
		// 初期化
		newShieldEnemy->Initialize(shieldenemyModel_, &camera_, mapChipField_->GetMapChipPositionByIndex(j, i));
		newShieldEnemy->SetGameScene(this);
		shieldenemies_.push_back(newShieldEnemy);

		break;
	default:
		break;
	}
}

void GameScene::CheckAllCollisions() {
#pragma region 自キャラと敵キャラの当たり判定
	// 判定対象1と2の座標
	AABBClass::AABB aabb1;
	AABBClass::AABB aabb2;

	// 自キャラの座標
	aabb1 = player_->GetAABB();

	// 自キャラと敵全ての当たり判定
	for (Enemy* enemy : enemies_) {
		if (enemy->IsCollisionDisabled()) {
			continue;
		}

		// 敵の座標
		aabb2 = enemy->GetAABB();

		// AABB同士の交差判定
		if (AABBClass::IsCollision(aabb1, aabb2)) {
			// 衝突応答
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision(enemy);
			// 敵の衝突時コールバックを呼び出す
			enemy->OnCollision(player_);
		}
	}

	for (ShieldEnemy* shieldEnemy : shieldenemies_) {
		if (shieldEnemy->IsCollisionDisabled()) {
			continue;
		}

		// 敵の座標
		aabb2 = shieldEnemy->GetAABB();

		// AABB同士の交差判定
		if (AABBClass::IsCollision(aabb1, aabb2)) {
			// 衝突応答
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnCollision(shieldEnemy);
			// 敵の衝突時コールバックを呼び出す
			shieldEnemy->OnCollision(player_);
		}
	}
#pragma endregion
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn:

		break;
	case Phase::kPlay:
		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			deathParticles_->Initialize(deathParticleModel_, &camera_, deathParticlesPosition);
		}
		break;
	case Phase::kDeath:
		break;
	case Phase::kFadeOut:
		break;
	}
}

void GameScene::CreateHitEffect(const KamataEngine::Vector3& occurrencePosition) {
	HitEffect* newHitEffect = HitEffect::Create(occurrencePosition);
	hitEffects_.push_back(newHitEffect);
}

void GameScene::CreateGuardEffect(const KamataEngine::Vector3& occurrencePosition) {
	GuardEffect* newGuardEffect = GuardEffect::Create(occurrencePosition);
	guardEffects_.push_back(newGuardEffect);
}

GameScene::~GameScene() {
	delete model_;

	delete block_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	// 自キャラの解放
	delete player_;

	// デスパーティクルの開放
	delete deathParticles_;

	delete debugCamera_;

	// 敵キャラの解放
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	for (ShieldEnemy* shieldEnemy : shieldenemies_) {
		delete shieldEnemy;
	}
	shieldenemies_.clear();

	// エフェクトの開放
	for (HitEffect* hitEffect : hitEffects_) {
		delete hitEffect;
	}
	hitEffects_.clear();

	for (GuardEffect* guardEffect : guardEffects_) {
		delete guardEffect;
	}
	guardEffects_.clear();

	// 天球の解放
	delete skydome_;

	delete modelSkydome_;

	// マップチップフィールドの解放
	delete mapChipField_;

	// 追従カメラの解放
	delete cameraFollow_;

	// フェードの開放
	delete fade_;

	// エフェクト用モデルの開放
	delete hitEffectModel_;
	delete guardEffectModel_;

	delete clearTextModel_;
}
