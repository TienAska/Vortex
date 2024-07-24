#pragma once

#include "Math.h"

namespace Vortex
{
    class Camera
    {
    public:
        Camera()
        {
        }
        
        inline Matrix GetView() const
        {
            DirectX::XMVECTOR forward = DirectX::XMVector3Rotate(DirectX::g_XMNegIdentityR1, m_rotation);
            DirectX::XMVECTOR up = DirectX::XMVector3Rotate(DirectX::g_XMIdentityR2, m_rotation);
            Matrix view = DirectX::XMMatrixLookToLH(m_position, forward, up);
            return view;
        }

        inline Matrix GetProjection() const
        {
            Matrix projection = DirectX::XMMatrixPerspectiveFovLH(90.0f, 1.0f, 0.0001f, 10000.0f);
            return projection;
        }

        inline void MoveForward(float offset)
        {
            DirectX::XMVECTOR forward = DirectX::XMVector3Rotate(DirectX::g_XMNegIdentityR1, m_rotation);
            m_position += DirectX::XMVectorScale(forward, offset);
        }
        
        inline void MoveRight(float offset)
        {
            DirectX::XMVECTOR up = DirectX::XMVector3Rotate(DirectX::g_XMIdentityR2, m_rotation);
            m_position += DirectX::XMVectorScale(up, offset);
        }

        inline void MoveUp(float offset)
        {
            DirectX::XMVECTOR right = DirectX::XMVector3Rotate(DirectX::g_XMIdentityR0, m_rotation);
            m_position += DirectX::XMVectorScale(right, offset);
        }

        inline void Yaw(float delta)
        {
            DirectX::XMVECTOR yawRotation = DirectX::XMQuaternionRotationRollPitchYaw(0, 0, delta);
            m_rotation = DirectX::XMQuaternionMultiply(m_rotation, yawRotation);
        }

        inline void Roll(float delta)
        {
            DirectX::XMVECTOR rollRotation = DirectX::XMQuaternionRotationRollPitchYaw(0, delta, 0);
            m_rotation = DirectX::XMQuaternionMultiply(m_rotation, rollRotation);
        }

        inline void Pitch(float delta)
        {
            DirectX::XMVECTOR pitchRotation = DirectX::XMQuaternionRotationRollPitchYaw(delta, 0, 0);
            m_rotation = DirectX::XMQuaternionMultiply(m_rotation, pitchRotation);
        }

        inline void Rotate(float yaw, float pitch)
        {
            DirectX::XMVECTOR rotation = DirectX::XMQuaternionRotationRollPitchYaw(pitch, 0, yaw);
            m_rotation = DirectX::XMQuaternionMultiply(m_rotation, rotation);
        }

    private:
        Vector3 m_position;
        Quaternion m_rotation;
    };
}