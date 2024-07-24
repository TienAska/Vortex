#pragma once

#include "Renderer.h"
#include "Shader.h"

namespace Vortex
{
    class SkyboxRenderPass : public Renderer::IRenderPass
    {
    public:
        SkyboxRenderPass()
        {
            //m_textureResource = VX_DEVICE0->CreateTextureResource(DXGI_FORMAT_R8_UNORM, 32 * 32, 32 * 32);
            //m_textureHanle = VX_DEVICE0->CreateSRV(4, m_textureResource, DXGI_FORMAT_R8_UNORM);
            //CD3DX12_ROOT_PARAMETER1 rootParameter;
            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc;
            versionedRootSignatureDesc.Init_1_1(0, nullptr);

            m_rootSignature = VX_DEVICE0->CreateRootSignature(versionedRootSignatureDesc);

            auto skyboxMS = std::make_shared<Shader>(L"SkyboxMS");
            auto skyboxPS = std::make_shared<Shader>(L"SkyboxPS");
            m_skyboxPSO = VX_DEVICE0->CreateMeshPSO(m_rootSignature, skyboxMS->GetBytecode(), skyboxPS->GetBytecode());
            
            m_commandAllocator = VX_DEVICE0->CreateGraphicsCommandAllocator();
            m_commandList = VX_DEVICE0->CreateGraphicsCommandList();
        }

        inline ID3D12GraphicsCommandList* GetCommandList(std::shared_ptr<SwapChain> swapChain, const Renderer& renderer) const override
        {
            winrt::check_hresult(m_commandAllocator->Reset());
            winrt::check_hresult(m_commandList->Reset(m_commandAllocator.get(), nullptr));

            m_commandList->RSSetViewports(1, swapChain->GetViewport());
            m_commandList->RSSetScissorRects(1, swapChain->GetScissorRect());

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain->GetBackBufferRTVHandle();
            m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

            m_commandList->SetPipelineState(m_skyboxPSO.get());
            m_commandList->DispatchMesh(1, 1, 1);

            winrt::check_hresult(m_commandList->Close());
            return m_commandList.get();
        }

    private:
        //winrt::com_ptr<ID3D12Resource> m_textureResource;
        //CD3DX12_GPU_DESCRIPTOR_HANDLE m_textureHanle;
        winrt::com_ptr<ID3D12RootSignature> m_rootSignature;
        winrt::com_ptr<ID3D12PipelineState> m_skyboxPSO;
        // Command
        winrt::com_ptr<ID3D12CommandAllocator> m_commandAllocator;
        winrt::com_ptr<ID3D12GraphicsCommandList6> m_commandList;
    };
}