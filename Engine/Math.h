#pragma once

namespace Vortex
{
    struct Matrix : public DirectX::XMFLOAT4X4
    {
        Matrix() noexcept : XMFLOAT4X4(
            1.f, 0, 0, 0,
            0, 1.f, 0, 0,
            0, 0, 1.f, 0,
            0, 0, 0, 1.f) {}
        Matrix(DirectX::CXMMATRIX M) noexcept { DirectX::XMStoreFloat4x4(this, M); }
        Matrix(const Matrix&) = default;
        Matrix& operator=(const Matrix&) = default;
        Matrix(Matrix&&) = default;
        Matrix& operator=(Matrix&&) = default;
        
        operator DirectX::XMMATRIX() const noexcept { return XMLoadFloat4x4(this); }
    };
    
    struct Vector3 : public DirectX::XMFLOAT3
    {
        Vector3() noexcept : XMFLOAT3(0.f, 0.f, 0.f) {};
        Vector3(DirectX::FXMVECTOR V) noexcept { XMStoreFloat3(this, V); }
        Vector3(const Vector3&) = default;
        Vector3& operator=(const Vector3&) = default;
        Vector3(Vector3&&) = default;
        Vector3& operator=(Vector3&&) = default;
        
        operator DirectX::XMVECTOR() const noexcept { return XMLoadFloat3(this); }

        inline Vector3& operator+= (const Vector3& V) noexcept
        {
            const DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(this);
            const DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&V);
            const DirectX::XMVECTOR X = DirectX::XMVectorAdd(v1, v2);
            DirectX::XMStoreFloat3(this, X);
            return *this;
        }
    };

    struct Quaternion : public DirectX::XMFLOAT4
    {
        Quaternion() noexcept : XMFLOAT4(0, 0, 0, 1.f) {}
        Quaternion(DirectX::FXMVECTOR V) noexcept { XMStoreFloat4(this, V); }
        Quaternion(const Quaternion&) = default;
        Quaternion& operator=(const Quaternion&) = default;
        Quaternion(Quaternion&&) = default;
        Quaternion& operator=(Quaternion&&) = default;
        
        operator DirectX::XMVECTOR() const noexcept { return XMLoadFloat4(this); }
    };
}