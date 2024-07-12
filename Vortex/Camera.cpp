#include "pch.h"

#include "Camera.h"



Vortex::Camera::Camera() :
	m_Position(0.0f, 0.0f, -2.0f), m_forward(0.0f, 0.0f, 1.0f), m_up(0.0f, 1.0f, 0.0f), m_right(1.0f, 0.0f, 0.0f),
	m_ProjectionMatrix(DirectX::XMMatrixPerspectiveFovLH(m_VFoV, m_Aspect, 0.1f, 1000.0f)),
	m_Rotation(DirectX::SimpleMath::Quaternion::Identity)
{

}
