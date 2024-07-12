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
            m_descriptorHeap = VX_DEVICE0->CreateResourceHeap(3);

            m_constantResource = VX_DEVICE0->CreateConstantResource(sizeof(GlobalParameters));
            m_handle0 = VX_DEVICE0->CreateCBV(m_constantResource, sizeof(GlobalParameters), m_descriptorHeap, 0);

            m_textureResource = VX_DEVICE0->CreateUnorderedResource(DXGI_FORMAT_R8_UNORM, 32, 32);
            VX_DEVICE0->CreateSRV(m_textureResource, DXGI_FORMAT_R8_UNORM, m_descriptorHeap, 1);
            
            m_handle1 = VX_DEVICE0->CreateUAV(m_textureResource, DXGI_FORMAT_R8_UNORM, m_descriptorHeap, 2);

            // Create pipeline.
            CD3DX12_DESCRIPTOR_RANGE1 descRange[3];
            descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); // b0
            descRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0
            descRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); // u0

            CD3DX12_ROOT_PARAMETER1 rootParameter[2];
            rootParameter[0].InitAsDescriptorTable(2, &descRange[0]); // b0, t0
            rootParameter[1].InitAsDescriptorTable(1, &descRange[2]); // u0

            CD3DX12_STATIC_SAMPLER_DESC staticSamplerDesc[1];
            staticSamplerDesc[0].Init(0); // s0

            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc;
            versionedRootSignatureDesc.Init_1_1(_countof(rootParameter), rootParameter, _countof(staticSamplerDesc), staticSamplerDesc);

            m_rootSignature = VX_DEVICE0->CreateRootSignature(versionedRootSignatureDesc);

            auto amplificationShader = std::make_shared<Shader>(L"ProceduralAS");
            auto meshShader = std::make_shared<Shader>(L"ProceduralMS");
            auto pixelShader = std::make_shared<Shader>(L"ProceduralPS");
            m_pipelineState = VX_DEVICE0->CreateMeshPSO(m_rootSignature, meshShader->GetBytecode(), pixelShader->GetBytecode(), amplificationShader->GetBytecode());

            m_commandAllocator = VX_DEVICE0->CreateBundleCommandAllocator();
            m_commandList = VX_DEVICE0->CreateBundleCommandList();

            // Record execution command bundle
            RecordExecution();
        }

        inline void RecordExecution() const
        {
            m_commandAllocator->Reset();
            m_commandList->Reset(m_commandAllocator.get(), m_pipelineState.get());

            ID3D12DescriptorHeap* heaps[] = { m_descriptorHeap.get() };
            m_commandList->SetDescriptorHeaps(_countof(heaps), heaps);
            m_commandList->SetGraphicsRootSignature(m_rootSignature.get());
            m_commandList->SetGraphicsRootDescriptorTable(0, m_handle0);
            m_commandList->SetGraphicsRootDescriptorTable(1, m_handle1);

            m_commandList->DispatchMesh(1, 1, 1);
            winrt::check_hresult(m_commandList->Close());
        }

        inline ID3D12GraphicsCommandList* GetCommandList() const override { return m_commandList.get(); }
        inline ID3D12DescriptorHeap* GetDescriptorHeap() const override { return m_descriptorHeap.get(); }

    private:
        // Resource
        winrt::com_ptr<ID3D12DescriptorHeap> m_descriptorHeap;
        winrt::com_ptr<ID3D12Resource> m_constantResource;
        winrt::com_ptr<ID3D12Resource> m_textureResource;
        CD3DX12_GPU_DESCRIPTOR_HANDLE m_handle0;
        CD3DX12_GPU_DESCRIPTOR_HANDLE m_handle1;

        // Pipeline
        winrt::com_ptr<ID3D12RootSignature> m_rootSignature;
        winrt::com_ptr<ID3D12PipelineState> m_pipelineState;
        // Command
        winrt::com_ptr<ID3D12CommandAllocator> m_commandAllocator;
        winrt::com_ptr<ID3D12GraphicsCommandList6> m_commandList;
    };
}