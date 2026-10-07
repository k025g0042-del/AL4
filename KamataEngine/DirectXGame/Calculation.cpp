#include "Calculation.h"

using namespace KamataEngine;

Matrix4x4 Calculation::Multiply(Matrix4x4 matrix1, Matrix4x4 matrix2) {
	Matrix4x4 result;

	result.m[0][0] = matrix1.m[0][0] * matrix2.m[0][0] + matrix1.m[0][1] * matrix2.m[1][0] + matrix1.m[0][2] * matrix2.m[2][0] + matrix1.m[0][3] * matrix2.m[3][0];
	result.m[0][1] = matrix1.m[0][0] * matrix2.m[0][1] + matrix1.m[0][1] * matrix2.m[1][1] + matrix1.m[0][2] * matrix2.m[2][1] + matrix1.m[0][3] * matrix2.m[3][1];
	result.m[0][2] = matrix1.m[0][0] * matrix2.m[0][2] + matrix1.m[0][1] * matrix2.m[1][2] + matrix1.m[0][2] * matrix2.m[2][2] + matrix1.m[0][3] * matrix2.m[3][2];
	result.m[0][3] = matrix1.m[0][0] * matrix2.m[0][3] + matrix1.m[0][1] * matrix2.m[1][3] + matrix1.m[0][2] * matrix2.m[2][3] + matrix1.m[0][3] * matrix2.m[3][3];

	result.m[1][0] = matrix1.m[1][0] * matrix2.m[0][0] + matrix1.m[1][1] * matrix2.m[1][0] + matrix1.m[1][2] * matrix2.m[2][0] + matrix1.m[1][3] * matrix2.m[3][0];
	result.m[1][1] = matrix1.m[1][0] * matrix2.m[0][1] + matrix1.m[1][1] * matrix2.m[1][1] + matrix1.m[1][2] * matrix2.m[2][1] + matrix1.m[1][3] * matrix2.m[3][1];
	result.m[1][2] = matrix1.m[1][0] * matrix2.m[0][2] + matrix1.m[1][1] * matrix2.m[1][2] + matrix1.m[1][2] * matrix2.m[2][2] + matrix1.m[1][3] * matrix2.m[3][2];
	result.m[1][3] = matrix1.m[1][0] * matrix2.m[0][3] + matrix1.m[1][1] * matrix2.m[1][3] + matrix1.m[1][2] * matrix2.m[2][3] + matrix1.m[1][3] * matrix2.m[3][3];

	result.m[2][0] = matrix1.m[2][0] * matrix2.m[0][0] + matrix1.m[2][1] * matrix2.m[1][0] + matrix1.m[2][2] * matrix2.m[2][0] + matrix1.m[2][3] * matrix2.m[3][0];
	result.m[2][1] = matrix1.m[2][0] * matrix2.m[0][1] + matrix1.m[2][1] * matrix2.m[1][1] + matrix1.m[2][2] * matrix2.m[2][1] + matrix1.m[2][3] * matrix2.m[3][1];
	result.m[2][2] = matrix1.m[2][0] * matrix2.m[0][2] + matrix1.m[2][1] * matrix2.m[1][2] + matrix1.m[2][2] * matrix2.m[2][2] + matrix1.m[2][3] * matrix2.m[3][2];
	result.m[2][3] = matrix1.m[2][0] * matrix2.m[0][3] + matrix1.m[2][1] * matrix2.m[1][3] + matrix1.m[2][2] * matrix2.m[2][3] + matrix1.m[2][3] * matrix2.m[3][3];

	result.m[3][0] = matrix1.m[3][0] * matrix2.m[0][0] + matrix1.m[3][1] * matrix2.m[1][0] + matrix1.m[3][2] * matrix2.m[2][0] + matrix1.m[3][3] * matrix2.m[3][0];
	result.m[3][1] = matrix1.m[3][0] * matrix2.m[0][1] + matrix1.m[3][1] * matrix2.m[1][1] + matrix1.m[3][2] * matrix2.m[2][1] + matrix1.m[3][3] * matrix2.m[3][1];
	result.m[3][2] = matrix1.m[3][0] * matrix2.m[0][2] + matrix1.m[3][1] * matrix2.m[1][2] + matrix1.m[3][2] * matrix2.m[2][2] + matrix1.m[3][3] * matrix2.m[3][2];
	result.m[3][3] = matrix1.m[3][0] * matrix2.m[0][3] + matrix1.m[3][1] * matrix2.m[1][3] + matrix1.m[3][2] * matrix2.m[2][3] + matrix1.m[3][3] * matrix2.m[3][3];

	return result;
}

Matrix4x4 Calculation::MakeRotateMatrix(Vector3 rotate) {
	Matrix4x4 result;

	Matrix4x4 rotateXMatrix;
	Matrix4x4 rotateYMatrix;
	Matrix4x4 rotateZMatrix;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			rotateXMatrix.m[i][j] = 0;
			rotateYMatrix.m[i][j] = 0;
			rotateZMatrix.m[i][j] = 0;
			result.m[i][j] = 0;
		}
	}

	rotateXMatrix.m[0][0] = 1.0f;
	rotateXMatrix.m[1][1] = cosf(rotate.x);
	rotateXMatrix.m[1][2] = sinf(rotate.x);
	rotateXMatrix.m[2][1] = -sinf(rotate.x);
	rotateXMatrix.m[2][2] = cosf(rotate.x);
	rotateXMatrix.m[3][3] = 1.0f;

	rotateYMatrix.m[0][0] = cosf(rotate.y);
	rotateYMatrix.m[0][2] = -sinf(rotate.y);
	rotateYMatrix.m[1][1] = 1.0f;
	rotateYMatrix.m[2][0] = sinf(rotate.y);
	rotateYMatrix.m[2][2] = cosf(rotate.y);
	rotateYMatrix.m[3][3] = 1.0f;

	rotateZMatrix.m[0][0] = cosf(rotate.z);
	rotateZMatrix.m[0][1] = sinf(rotate.z);
	rotateZMatrix.m[1][0] = -sinf(rotate.z);
	rotateZMatrix.m[1][1] = cosf(rotate.z);
	rotateZMatrix.m[2][2] = 1.0f;
	rotateZMatrix.m[3][3] = 1.0f;

	result = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	return result;
}

Matrix4x4 Calculation::MakeRotateZMatrix(float rotate) {
	Matrix4x4 result;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = 0;
		}
	}

	result.m[0][0] = cosf(rotate);
	result.m[0][1] = sinf(rotate);
	result.m[1][0] = -sinf(rotate);
	result.m[1][1] = cosf(rotate);
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 Calculation::MakeTranslateMatrix(Vector3 translate) {
	Matrix4x4 result;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = 0;
		}
	}

	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 Calculation::MakeScaleMatrix(Vector3 scale) {
	Matrix4x4 result;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = 0;
		}
	}

	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 Calculation::MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate) {
	Matrix4x4 result;

	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotate);
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);

	result = Multiply(scaleMatrix, rotateMatrix);
	result = Multiply(result, translateMatrix);

	return result;
}

Vector3 Calculation::Transform(Vector3 vector, Matrix4x4 matrix) {
	Vector3 result;

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}
