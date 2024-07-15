#pragma once

#include "Renderer.h"
#include "Shader.h"

namespace Vortex
{
    class MeshRenderPass : public Renderer::IRenderPass
    {
    public:
        MeshRenderPass()
        {
            // Create resources.
            {
                m_descriptorHeap = VX_DEVICE0->CreateResourceHeap(3);

                m_constantResource = VX_DEVICE0->CreateConstantResource(sizeof(GlobalParameters));
                m_gpuHandle0 = VX_DEVICE0->CreateCBV(m_descriptorHeap, 0, m_constantResource, sizeof(GlobalParameters));

                m_textureResource = VX_DEVICE0->CreateTextureResource(DXGI_FORMAT_R8_UNORM, 32, 32);
                m_gpuHandle1 = VX_DEVICE0->CreateSRV(m_descriptorHeap, 1, m_textureResource, DXGI_FORMAT_R8_UNORM);

                m_gpuHandle2 = VX_DEVICE0->CreateUAV(m_descriptorHeap, 2, m_textureResource, DXGI_FORMAT_R8_UNORM);
            }
            // Create root signature.
            {
                CD3DX12_DESCRIPTOR_RANGE1 descRange[3];
                descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); // b0
                descRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0
                descRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); // u0

                CD3DX12_ROOT_PARAMETER1 rootParameter[3];
                rootParameter[0].InitAsDescriptorTable(1, &descRange[0]); // b0
                rootParameter[1].InitAsDescriptorTable(1, &descRange[1]); // t0
                rootParameter[2].InitAsDescriptorTable(1, &descRange[2]); // u0

                CD3DX12_STATIC_SAMPLER_DESC staticSamplerDesc[2];
                staticSamplerDesc[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_POINT); // s0
                staticSamplerDesc[1].Init(1, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // s1

                CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc;
                versionedRootSignatureDesc.Init_1_1(_countof(rootParameter), rootParameter, _countof(staticSamplerDesc), staticSamplerDesc);

                m_rootSignature = VX_DEVICE0->CreateRootSignature(versionedRootSignatureDesc);
            }
            // Create pipeline states.
            {
                auto computeShader = std::make_shared<Shader>(L"NoiseCS");
                m_computePSO = VX_DEVICE0->CreateComputePSO(m_rootSignature, computeShader->GetBytecode());

                auto amplificationShader = std::make_shared<Shader>(L"ProceduralAS");
                auto meshShader = std::make_shared<Shader>(L"ProceduralMS");
                auto pixelShader = std::make_shared<Shader>(L"ProceduralPS");
                m_graphicsPSO = VX_DEVICE0->CreateMeshPSO(m_rootSignature, meshShader->GetBytecode(), pixelShader->GetBytecode(), amplificationShader->GetBytecode());

                m_commandAllocator = VX_DEVICE0->CreateGraphicsCommandAllocator();
                m_commandList = VX_DEVICE0->CreateGraphicsCommandList();
            }
        }

        inline ID3D12GraphicsCommandList* GetCommandList(std::shared_ptr<SwapChain> swapChain) const override {
            winrt::check_hresult(m_commandAllocator->Reset());
            winrt::check_hresult(m_commandList->Reset(m_commandAllocator.get(), nullptr));

            m_commandList->RSSetViewports(1, swapChain->GetViewport());
            m_commandList->RSSetScissorRects(1, swapChain->GetScissorRect());

            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain->GetBackBufferRTVHandle();
            m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

            ID3D12DescriptorHeap* heaps[] = { m_descriptorHeap.get() };
            m_commandList->SetDescriptorHeaps(_countof(heaps), heaps);

            // Compute
            auto transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_textureResource.get(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
            m_commandList->ResourceBarrier(1, &transitionBarrier);

            m_commandList->SetPipelineState(m_computePSO.get());
            m_commandList->SetComputeRootSignature(m_rootSignature.get());
            m_commandList->SetComputeRootDescriptorTable(2, m_gpuHandle2);
            m_commandList->Dispatch(1, 1, 1);

            // Graphics
            transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_textureResource.get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
            m_commandList->ResourceBarrier(1, &transitionBarrier);
            m_commandList->SetPipelineState(m_graphicsPSO.get());
            m_commandList->SetGraphicsRootSignature(m_rootSignature.get());
            m_commandList->SetGraphicsRootDescriptorTable(0, m_gpuHandle0);
            m_commandList->SetGraphicsRootDescriptorTable(1, m_gpuHandle1);
            m_commandList->DispatchMesh(1, 1, 1);

            winrt::check_hresult(m_commandList->Close());

            return m_commandList.get();
        }

        inline ID3D12DescriptorHeap* GetDescriptorHeap() const override { return m_descriptorHeap.get(); }
        //inline uint32_t GetResourceBarrier(const CD3DX12_RESOURCE_BARRIER*& barrier) const override
        //{
        //    barrier = new CD3DX12_RESOURCE_BARRIER(CD3DX12_RESOURCE_BARRIER::Transition(m_textureResource.get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
        //    return 1;
        //}

    private:
        // Resource
        winrt::com_ptr<ID3D12DescriptorHeap> m_descriptorHeap;
        winrt::com_ptr<ID3D12Resource> m_constantResource;
        winrt::com_ptr<ID3D12Resource> m_textureResource;
        CD3DX12_GPU_DESCRIPTOR_HANDLE m_gpuHandle0;
        CD3DX12_GPU_DESCRIPTOR_HANDLE m_gpuHandle1;
        CD3DX12_GPU_DESCRIPTOR_HANDLE m_gpuHandle2;

        // Pipeline
        winrt::com_ptr<ID3D12RootSignature> m_rootSignature;
        winrt::com_ptr<ID3D12PipelineState> m_computePSO;
        winrt::com_ptr<ID3D12PipelineState> m_graphicsPSO;
        // Command
        winrt::com_ptr<ID3D12CommandAllocator> m_commandAllocator;
        winrt::com_ptr<ID3D12GraphicsCommandList6> m_commandList;
    };
}