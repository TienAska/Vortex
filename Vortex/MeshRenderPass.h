#pragma once

namespace Vortex
{
    winrt::com_ptr<ID3D12PipelineState> CreatePreceduralMeshPSO(winrt::com_ptr<ID3D12Resource> renderTarget, winrt::com_ptr<ID3D12Resource> depthStencil)
    {
        winrt::com_ptr<ID3D12Device2> device = DeviceManager::Instance().GetDevice();

        winrt::com_ptr<ID3D12RootSignature> rootSignature; // Empty
        D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
        rootSigDesc.NumParameters = 0;
        rootSigDesc.pParameters = nullptr;
        rootSigDesc.NumStaticSamplers = 0;
        rootSigDesc.pStaticSamplers = nullptr;
        rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        winrt::com_ptr<ID3DBlob> serializedRootSig = nullptr;
        winrt::com_ptr<ID3DBlob> errorBlob = nullptr;
        HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.put(), errorBlob.put());

        if (SUCCEEDED(hr)) {
            device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
        }

        auto meshShader = std::make_shared<Shader>(L"ProceduralMS");
        auto pixelShader = std::make_shared<Shader>(L"ProceduralPS");

        D3DX12_MESH_SHADER_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature = rootSignature.get();
        psoDesc.AS = { NULL, 0 };
        psoDesc.MS = meshShader->GetBytecode();
        psoDesc.PS = pixelShader->GetBytecode();
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);         // Opaque
        psoDesc.BlendState.RenderTarget[0].BlendEnable = true;        // Transparency
        //psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        //psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        //psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        psoDesc.SampleMask = DefaultSampleMask();
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);    // CW front; cull back
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // Less-equal depth test w/ writes; no stencil
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = renderTarget->GetDesc().Format;
        //psoDesc.DSVFormat = depthStencil->GetDesc().Format;
        psoDesc.SampleDesc = DefaultSampleDesc();
        psoDesc.NodeMask = 0;
        psoDesc.CachedPSO = { NULL, 0 };
        psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

        auto psoStream = CD3DX12_PIPELINE_MESH_STATE_STREAM(psoDesc);

        D3D12_PIPELINE_STATE_STREAM_DESC streamDesc;
        streamDesc.pPipelineStateSubobjectStream = &psoStream;
        streamDesc.SizeInBytes = sizeof(psoStream);

        winrt::com_ptr<ID3D12PipelineState> pipelineState;
        winrt::check_hresult(device->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&pipelineState)));

        return pipelineState;
    }
}