#pragma once
#include "KamataEngine.h"

class Calculation {
public:
	/*
	//	行列
	//————————————————————————————————————————————————————————————————————————————————————————*/

	/// <summary>
	/// 行列同士の掛け算
	/// </summary>
	/// <param name="matrix1">行列１</param>
	/// <param name="matrix2">行列２</param>
	/// <returns></returns>
	static KamataEngine::Matrix4x4 Multiply(KamataEngine::Matrix4x4 matrix1, KamataEngine::Matrix4x4 matrix2);

	/// <summary>
	/// 回転行列の作成
	/// </summary>
	/// <param name="rotate">回転</param>
	/// <returns></returns>
	static KamataEngine::Matrix4x4 MakeRotateMatrix(KamataEngine::Vector3 rotate);

	/// <summary>
	/// Z軸まわり回転行列の作成
	/// </summary>
	/// <param name="rotate">回転</param>
	/// <returns></returns>
	static KamataEngine::Matrix4x4 MakeRotateZMatrix(float rotate);

	/// <summary>
	/// 平行移動行列の作成
	/// </summary>
	/// <param name="translate">移動量</param>
	/// <returns></returns>
	static KamataEngine::Matrix4x4 MakeTranslateMatrix(KamataEngine::Vector3 translate);

	/// <summary>
	/// 拡大縮小行列の作成
	/// </summary>
	/// <param name="scale">拡縮</param>
	/// <returns></returns>
	static KamataEngine::Matrix4x4 MakeScaleMatrix(KamataEngine::Vector3 scale);

	/// <summary>
	/// アフィン変換
	/// </summary>
	/// <param name="scale">拡縮</param>
	/// <param name="rotate">回転</param>
	/// <param name="translate">移動</param>
	/// <returns></returns>
	static KamataEngine::Matrix4x4 MakeAffineMatrix(KamataEngine::Vector3 scale, KamataEngine::Vector3 rotate, KamataEngine::Vector3 translate);

	/// <summary>
	/// 同時座標からデカルト座標への変換
	/// </summary>
	/// <param name="vector"></param>
	/// <param name="matrix"></param>
	/// <returns></returns>
	static KamataEngine::Vector3 Transform(KamataEngine::Vector3 vector, KamataEngine::Matrix4x4 matrix);
};
