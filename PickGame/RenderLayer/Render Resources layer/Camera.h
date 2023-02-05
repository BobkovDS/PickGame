#pragma once

#include "..\..\Include\datatypedef.h"
#include "DirectXMath.h"

#include <d3d12.h>


class Camera
{
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_right;
	DirectX::XMFLOAT3 m_up;
	DirectX::XMFLOAT3 m_look;
	DirectX::XMFLOAT4X4 m_viewMatrix;

	bool m_viewToUpdate;
	
	Camera(const Camera& c) = delete;

public:

	Camera();
	~Camera();

	Camera& operator=(const Camera&);

	class CameraLens;
	CameraLens* lens;

	DirectX::XMVECTOR getPosition() const;
	const DirectX::XMFLOAT3& getPosition3f() const;
	void setPosition(float x, float y, float z);
	void setPosition(const DirectX::XMFLOAT3& v);

	DirectX::XMVECTOR getRight() const;
	const DirectX::XMFLOAT3& getRight3f() const;
	DirectX::XMVECTOR getUp() const;
	const DirectX::XMFLOAT3& getUp3f() const;
	DirectX::XMVECTOR getLook() const;
	const DirectX::XMFLOAT3& getLook3f() const;
	DirectX::XMFLOAT3 getTarget3f() const;

	// Define camera space via LookAt parametrs
	void lookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void lookAt(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 worldUp);
	void lookTo(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR lookDir, DirectX::FXMVECTOR worldUp);

	//Update/Get View matrix
	DirectX::XMMATRIX getView();
	void updateViewMatrix();
	const DirectX::XMFLOAT4X4& getView4x4f() const;

	//Get Projection matrix
	const DirectX::XMFLOAT4X4& getProj4x4f() const;

	// Camera moving functions
	void strafe(float v);
	void walk(float v);
	void pitch(float v);
	void rotateY(float v);
	void transform(DirectX::XMFLOAT4X4& transformM);

	// Internal class for Basic class lens
	class CameraLens
	{
		CameraLens(const CameraLens&) = delete;

	protected:
		DirectX::XMFLOAT4X4 m_projectionMatrix;
		bool m_lensWasSet;

	public:
		CameraLens() :
			m_lensWasSet(false)
		{
			m_projectionMatrix = {};
		}

		DirectX::XMMATRIX getProj() const;
		const DirectX::XMFLOAT4X4& getProj4x4f() const;
		
		virtual void setLens(float p1, float p2, float p3, float p4) = 0;
		virtual void setLens(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 extents) = 0;
		virtual ~CameraLens(){};
	};
};

class PerspectiveCameraLens : public Camera::CameraLens
{
	float m_nearZ;
	float m_farZ;
	float m_aspect;
	float m_fovY;
	float m_nearWindowHeight;
	float m_farWindowHeight;

	PerspectiveCameraLens(const PerspectiveCameraLens&) = delete;
public:
	PerspectiveCameraLens();

	PerspectiveCameraLens& operator=(const PerspectiveCameraLens& c);

	void setLens(float fovY, float aspect, float zn, float zf);
	void setLens(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 extents) {};
	void setAspectRatio(float aspectRatio);
};

class OrthographicCameraLens : public Camera::CameraLens
{
	OrthographicCameraLens(const OrthographicCameraLens&) = delete;
public:

	OrthographicCameraLens() {};
	OrthographicCameraLens& operator=(const OrthographicCameraLens& c);
	void setLens(float p1, float p2, float p3, float p4);
	void setLens(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 extents);
};

// -------------- Inline Functions --------------
inline DirectX::XMVECTOR Camera::getPosition() const
{
	return DirectX::XMLoadFloat3(&m_position);
}

inline DirectX::XMVECTOR Camera::getLook() const
{
	return DirectX::XMLoadFloat3(&m_look);
}

inline const DirectX::XMFLOAT4X4& Camera::getView4x4f() const
{
	return m_viewMatrix;
}