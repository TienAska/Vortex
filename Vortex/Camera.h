#pragma once

namespace Vortex
{
	class Camera
	{
	public:
		Camera();

		DirectX::SimpleMath::Matrix GetViewMatrix()
		{
			return DirectX::SimpleMath::Matrix::CreateTranslation(m_Position) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation).Invert();
		}

		DirectX::SimpleMath::Matrix GetProjectionMatrix()
		{
			return m_ProjectionMatrix;
		}

		DirectX::SimpleMath::Matrix GetViewProjection()
		{
			return m_ProjectionMatrix * GetViewMatrix();
		}

		DirectX::SimpleMath::Matrix GetRotationMatrix()
		{
			return DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_Rotation).Invert();
		}

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
			DirectX::SimpleMath::Vector3 direction = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f), m_Rotation);
			m_Position += direction * offset;
			return *this;
		}

		Camera& MoveRight(float offset)
		{
			DirectX::SimpleMath::Vector3 direction = DirectX::SimpleMath::Vector3::Transform(DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f), m_Rotation);
			m_Position += direction * offset;
			return *this;
		}

		Camera& MoveUp(float offset)
		{
			DirectX::SimpleMath::Vector3 direction = m_Rotation * DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f);
			m_Position += direction * offset;
			return *this;
		}

		Camera& Rotate(float pitch, float yaw)
		{
			//m_Rotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), DirectX::XMConvertToRadians(-yaw * 0.1f)) * DirectX::SimpleMath::Quaternion::FromToRotation() rotate(m_Rotation, glm::radians(pitch * 0.1f), DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f));
			m_Rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, pitch, 0.0f);
			return *this;
		}

	private:
		float m_VFoV = DirectX::XM_PIDIV4;
		float m_Aspect = 16.0f / 9.0f;

		DirectX::SimpleMath::Vector3 m_Position;
		DirectX::SimpleMath::Quaternion m_Rotation;

		DirectX::SimpleMath::Matrix m_ProjectionMatrix;
	};
}