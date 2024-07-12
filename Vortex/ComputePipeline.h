#pragma once

namespace Vortex
{
    class ComputePipeline
    {
    public:
        ComputePipeline()
        {
            m_rootSignature = CreateRootSignature();
            m_pipelineState = CreatePipelineState(m_rootSignature);
            m_rwTexture = CreateRWTexture();
            m_descriptorHeap = CreateDescriptorHeap(m_rwTexture);
            m_commandList = CreateCommandList(m_pipelineState, m_rootSignature, m_descriptorHeap, m_rwTexture);
        }

        winrt::com_ptr<ID3D12RootSignature> GetRootSignature()const { return m_rootSignature; }
        winrt::com_ptr<ID3D12PipelineState> GetPipelineState()const { return m_pipelineState; }
        winrt::com_ptr<ID3D12DescriptorHeap> GetDescriptorHeap()const { return m_descriptorHeap; }
        winrt::com_ptr<ID3D12GraphicsCommandList> GetCommandList()const { return m_commandList; }

    private:
        winrt::com_ptr<ID3D12RootSignature> m_rootSignature;
        winrt::com_ptr<ID3D12PipelineState> m_pipelineState;
        winrt::com_ptr<ID3D12Resource> m_rwTexture;
        winrt::com_ptr<ID3D12DescriptorHeap> m_descriptorHeap;
        winrt::com_ptr<ID3D12GraphicsCommandList> m_commandList;

        static winrt::com_ptr<ID3D12RootSignature> CreateRootSignature()
        {
            winrt::com_ptr<ID3D12Device2> device = DeviceManager::Instance().GetDevice();

            CD3DX12_DESCRIPTOR_RANGE1 descRange[1];
            descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

            CD3DX12_ROOT_PARAMETER1 rootParameter[1];
            rootParameter[0].InitAsDescriptorTable(1, descRange);

            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc;
            versionedRootSignatureDesc.Init_1_1(1, rootParameter);

            winrt::com_ptr<ID3DBlob> signature;
            winrt::com_ptr<ID3DBlob> error;
            winrt::check_hresult(D3D12SerializeVersionedRootSignature(&versionedRootSignatureDesc, signature.put(), error.put()));

            winrt::com_ptr<ID3D12RootSignature> rootSignature;
            winrt::check_hresult(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
            return rootSignature;
        }

        static winrt::com_ptr<ID3D12PipelineState> CreatePipelineState(const winrt::com_ptr<ID3D12RootSignature>& rootSignature)
        {
            winrt::com_ptr<ID3D12Device2> device = DeviceManager::Instance().GetDevice();

            auto computeShader = std::make_shared<Shader>(L"NoiseCS");

            D3D12_COMPUTE_PIPELINE_STATE_DESC computeDesc = {};
            computeDesc.pRootSignature = rootSignature.get();
            computeDesc.CS = computeShader->GetBytecode();
            computeDesc.NodeMask = 0;
            computeDesc.CachedPSO = { NULL, 0 };
            computeDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

            winrt::com_ptr<ID3D12PipelineState> pipelineState;
            winrt::check_hresult(device->CreateComputePipelineState(&computeDesc, IID_PPV_ARGS(&pipelineState)));
            return pipelineState;
        }

        static winrt::com_ptr<ID3D12Resource> CreateRWTexture()
        {
            winrt::com_ptr<ID3D12Device2> device = DeviceManager::Instance().GetDevice();

            D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8_UNORM, 256, 256, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
            winrt::com_ptr<ID3D12Resource> texture2D;
            winrt::check_hresult(device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&texture2D)));
            texture2D->SetName(L"T_Noise");
            return texture2D;
        }

        static winrt::com_ptr<ID3D12DescriptorHeap> CreateDescriptorHeap(const winrt::com_ptr<ID3D12Resource>& rwTexture)
        {
            winrt::com_ptr<ID3D12Device2> device = DeviceManager::Instance().GetDevice();

            // Create descriptor heap.
            D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
            heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            heapDesc.NumDescriptors = 1;
            heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            heapDesc.NodeMask = 0;
            winrt::com_ptr<ID3D12DescriptorHeap> descriptorHeap;
            winrt::check_hresult(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap)));

            // Create UAV.
            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.Format = DXGI_FORMAT_R8_UNORM;
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Texture2D.MipSlice = 0;
            uavDesc.Texture2D.PlaneSlice = 0;
            device->CreateUnorderedAccessView(rwTexture.get(), nullptr, &uavDesc, descriptorHeap->GetCPUDescriptorHandleForHeapStart());

            return descriptorHeap;
        }

        static winrt::com_ptr<ID3D12GraphicsCommandList> CreateCommandList(
            const winrt::com_ptr<ID3D12PipelineState>& pipelineState,
            const winrt::com_ptr<ID3D12RootSignature>& rootSignature,
            const winrt::com_ptr<ID3D12DescriptorHeap>& descriptorHeap,
            const winrt::com_ptr<ID3D12Resource>& rwTexture
        )
        {
            winrt::com_ptr<ID3D12Device4> device = DeviceManager::Instance().GetDevice().as<ID3D12Device4>();

            winrt::com_ptr<ID3D12CommandAllocator> allocator;
            winrt::check_hresult(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)));
            winrt::com_ptr<ID3D12GraphicsCommandList> commandList;
            winrt::check_hresult(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.get(), pipelineState.get(), IID_PPV_ARGS(&commandList)));
            
            ID3D12DescriptorHeap* heap[] = { descriptorHeap.get() };
            commandList->SetDescriptorHeaps(1, heap);
            commandList->SetComputeRootSignature(rootSignature.get());
            commandList->SetComputeRootDescriptorTable(0, descriptorHeap->GetGPUDescriptorHandleForHeapStart());
            commandList->Dispatch(256 / 32, 256 / 32, 1);
            //auto transition = CD3DX12_RESOURCE_BARRIER::Transition(rwTexture.get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            //commandList->ResourceBarrier(1, &transition);
            winrt::check_hresult(commandList->Close());

            return commandList;
        }
    };
}