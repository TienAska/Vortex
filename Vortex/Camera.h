#pragma once

namespace Vortex
{
	class Camera
	{
	public:
		Camera();

		DirectX::SimpleMath::Matrix GetViewMatrix()
		{
			return DirectX::XMMatrixLookToLH(m_Position, m_forward, m_up);
		}

		DirectX::SimpleMath::Matrix GetProjectionMatrix()
		{
			return m_ProjectionMatrix;
		}

		DirectX::SimpleMath::Matrix GetViewProjection()
		{
			return m_ProjectionMatrix * GetViewMatrix();
		}

		//DirectX::SimpleMath::Matrix GetRotationMatrix()
		//{
		//	return DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation).Invert();
		//}

		DirectX::SimpleMath::Vector3 GetPosition()
		{
			return m_Position;
		}

		Camera& SetFoV(float vfov)
		{
			m_VFoV = vfov;
			return *this;
		}

		Camera& SetAspect(float aspect)
		{
			m_Aspect = aspect;
			return *this;
		}

		Camera& MoveForward(float offset)
		{
			m_Position += m_forward * offset;
			return *this;
		}

		Camera& MoveRight(float offset)
		{
			m_Position += m_right * offset;
			return *this;
		}

		Camera& MoveUp(float offset)
		{
			m_Position += m_up * offset;
			return *this;
		}

		Camera& Rotate(float pitch, float yaw)
		{
			
			//m_Rotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), DirectX::XMConvertToRadians(-yaw * 0.1f)) * DirectX::SimpleMath::Quaternion::FromToRotation() rotate(m_Rotation, glm::radians(pitch * 0.1f), DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f));
			DirectX::SimpleMath::Quaternion rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(pitch), 0.0f);
			m_forward = DirectX::SimpleMath::Vector3::Transform(m_forward, rotation);
			m_up = DirectX::SimpleMath::Vector3::Transform(m_up, rotation);
			m_right = DirectX::SimpleMath::Vector3::Transform(m_right, rotation);
			//m_Rotation = DirectX::SimpleMath::Quaternion::LookRotation(m_forward, m_up);
			return *this;
		}

	private:
		float m_VFoV = DirectX::XM_PIDIV4;
		float m_Aspect = 16.0f / 9.0f;

		DirectX::SimpleMath::Vector3 m_Position;
		DirectX::SimpleMath::Vector3 m_forward;
		DirectX::SimpleMath::Vector3 m_up;
		DirectX::SimpleMath::Vector3 m_right;

		DirectX::SimpleMath::Matrix m_ProjectionMatrix;
	};
}