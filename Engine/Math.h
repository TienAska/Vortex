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
    };
    
    struct Vector3 : public DirectX::XMFLOAT3
    {
        Vector3() noexcept : XMFLOAT3(0.f, 0.f, 0.f) {};
        Vector3(DirectX::FXMVECTOR V) noexcept { XMStoreFloat3(this, V); }
        Vector3(const Vector3&) = default;
        Vector3& operator=(const Vector3&) = default;
        Vector3(Vector3&&) = default;
        Vector3& operator=(Vector3&&) = default;
    };
}