#pragma once

namespace Vortex
{
    winrt::com_ptr<ID3D12PipelineState> CreatePreceduralMeshPSO(winrt::com_ptr<ID3D12RootSignature> rootSignature, winrt::com_ptr<ID3D12Resource> renderTarget, winrt::com_ptr<ID3D12Resource> depthStencil)
    {
        winrt::com_ptr<ID3D12Device2> device = DeviceManager::Instance().GetDevice();

        auto amplificationShader = std::make_shared<Shader>(L"ProceduralAS");
        auto meshShader = std::make_shared<Shader>(L"ProceduralMS");
        auto pixelShader = std::make_shared<Shader>(L"ProceduralPS");

        D3DX12_MESH_SHADER_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature = rootSignature.get();
        psoDesc.AS = amplificationShader->GetBytecode();
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

    winrt::com_ptr<ID3D12PipelineState> CreateVertexPixelPSO(winrt::com_ptr<ID3D12Resource> renderTarget, winrt::com_ptr<ID3D12Resource> depthStencil)
    {
        winrt::com_ptr<ID3D12Device2> device = DeviceManager::Instance().GetDevice();

        winrt::com_ptr<ID3D12RootSignature> rootSignature; // Empty
        // Create root signature.
        CD3DX12_DESCRIPTOR_RANGE1 descRange[1];
        descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

        CD3DX12_ROOT_PARAMETER1 rootParameter[1];
        //rootParameter[0].InitAsDescriptorTable(1, descRange);
        rootParameter[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC);


        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc;
        versionedRootSignatureDesc.Init_1_1(1, rootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        winrt::com_ptr<ID3DBlob> signature;
        winrt::com_ptr<ID3DBlob> error;

        winrt::check_hresult(D3D12SerializeVersionedRootSignature(&versionedRootSignatureDesc, signature.put(), error.put()));
        winrt::check_hresult(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));

        // Compile shaders.
        Shader vertexShader(L"TriangleVS");
        Shader pixelShader(L"TrianglePS");


        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
        	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };


        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = rootSignature.get();
        psoDesc.VS = vertexShader.GetBytecode();
        psoDesc.PS = pixelShader.GetBytecode();
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = renderTarget->GetDesc().Format;
        //psoDesc.DSVFormat = m_depthStencil->GetDesc().Format;
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);    // CW front; cull back
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);         // Opaque
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // Less-equal depth test w/ writes; no stencil
        //psoDesc.DepthStencilState.DepthEnable = FALSE;
        //psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.SampleDesc = DefaultSampleDesc();
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

        winrt::com_ptr<ID3D12PipelineState> pipelineState;
        winrt::check_hresult(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
        return pipelineState;
    }

    winrt::com_ptr<ID3D12RootSignature> CreateRootSignature()
    {
        winrt::com_ptr<ID3D12Device2> device = DeviceManager::Instance().GetDevice();

        winrt::com_ptr<ID3D12RootSignature> rootSignature; // Empty
        // Create root signature.
        CD3DX12_DESCRIPTOR_RANGE1 descRange[1];
        descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

        CD3DX12_ROOT_PARAMETER1 rootParameter[1];
        rootParameter[0].InitAsDescriptorTable(1, descRange);
        //rootParameter[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC);


        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc;
        versionedRootSignatureDesc.Init_1_1(1, rootParameter);

        winrt::com_ptr<ID3DBlob> signature;
        winrt::com_ptr<ID3DBlob> error;

        winrt::check_hresult(D3D12SerializeVersionedRootSignature(&versionedRootSignatureDesc, signature.put(), error.put()));
        winrt::check_hresult(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));

        return rootSignature;
    }
}