#include "pch.h"

#include "Camera.h"



Vortex::Camera::Camera():
	m_Position(0.0f, 0.0f, 0.0f), m_Rotation(DirectX::SimpleMath::Quaternion::Identity), m_ProjectionMatrix(DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(m_VFoV, m_Aspect, 0.1f, 1000.0f))
{

}
