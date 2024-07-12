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
            //CD3DX12_DESCRIPTOR_RANGE1 descRange[2];
            //descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
            //descRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

            //CD3DX12_ROOT_PARAMETER1 rootParameter[1];
            //rootParameter[0].InitAsDescriptorTable(_countof(descRange), descRange);

            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc;
            //versionedRootSignatureDesc.Init_1_1(_countof(rootParameter), rootParameter);
            versionedRootSignatureDesc.Init_1_1(0, nullptr);

            m_rootSignature = VX_DEVICE0->CreateRootSignature(versionedRootSignatureDesc);


            //auto amplificationShader = std::make_shared<Shader>(L"ProceduralAS");
            auto meshShader = std::make_shared<Shader>(L"TriangleMS");
            auto pixelShader = std::make_shared<Shader>(L"TrianglePS");

            m_pipelineState = VX_DEVICE0->CreateMeshPSO(m_rootSignature, meshShader->GetBytecode(), pixelShader->GetBytecode());


            m_commandAllocator = VX_DEVICE0->CreateBundleCommandAllocator();
            m_commandList = VX_DEVICE0->CreateBundleCommandList();

            m_commandAllocator->Reset();
            m_commandList->Reset(m_commandAllocator.get(), m_pipelineState.get());
            //ID3D12DescriptorHeap* heaps[] = { descriptorHeap.get() };
            //m_commandList->SetDescriptorHeaps(_countof(heaps), heaps);
            m_commandList->SetGraphicsRootSignature(m_rootSignature.get());
            //m_commandList->SetGraphicsRootDescriptorTable(0, descriptorHeap->GetGPUDescriptorHandleForHeapStart());

            // Record commands.
            m_commandList->DispatchMesh(1, 1, 1);
            winrt::check_hresult(m_commandList->Close());
        }

        inline ID3D12GraphicsCommandList* GetCommandList() const override { return m_commandList.get(); }

    private:

        winrt::com_ptr<ID3D12RootSignature> m_rootSignature;
        winrt::com_ptr<ID3D12PipelineState> m_pipelineState;
        winrt::com_ptr<ID3D12CommandAllocator> m_commandAllocator;
        winrt::com_ptr<ID3D12GraphicsCommandList6> m_commandList;
    };

    //winrt::com_ptr<ID3D12PipelineState> CreateVertexPixelPSO(winrt::com_ptr<ID3D12Resource> renderTarget, winrt::com_ptr<ID3D12Resource> depthStencil)
//{
//    winrt::com_ptr<ID3D12Device2> device = DeviceManager::Instance().GetDevice();

//    winrt::com_ptr<ID3D12RootSignature> rootSignature; // Empty
//    // Create root signature.
//    CD3DX12_DESCRIPTOR_RANGE1 descRange[1];
//    descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

//    CD3DX12_ROOT_PARAMETER1 rootParameter[1];
//    //rootParameter[0].InitAsDescriptorTable(1, descRange);
//    rootParameter[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC);


//    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc;
//    versionedRootSignatureDesc.Init_1_1(1, rootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

//    winrt::com_ptr<ID3DBlob> signature;
//    winrt::com_ptr<ID3DBlob> error;

//    winrt::check_hresult(D3D12SerializeVersionedRootSignature(&versionedRootSignatureDesc, signature.put(), error.put()));
//    winrt::check_hresult(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));

//    // Compile shaders.
//    Shader vertexShader(L"TriangleVS");
//    Shader pixelShader(L"TrianglePS");


//    // Define the vertex input layout.
//    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
//    {
//    	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//    	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
//    };


//    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
//    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
//    psoDesc.pRootSignature = rootSignature.get();
//    psoDesc.VS = vertexShader.GetBytecode();
//    psoDesc.PS = pixelShader.GetBytecode();
//    psoDesc.NumRenderTargets = 1;
//    psoDesc.RTVFormats[0] = renderTarget->GetDesc().Format;
//    //psoDesc.DSVFormat = m_depthStencil->GetDesc().Format;
//    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);    // CW front; cull back
//    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);         // Opaque
//    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // Less-equal depth test w/ writes; no stencil
//    //psoDesc.DepthStencilState.DepthEnable = FALSE;
//    //psoDesc.DepthStencilState.StencilEnable = FALSE;
//    psoDesc.SampleMask = UINT_MAX;
//    psoDesc.SampleDesc = DefaultSampleDesc();
//    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

//    winrt::com_ptr<ID3D12PipelineState> pipelineState;
//    winrt::check_hresult(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
//    return pipelineState;
//}
}