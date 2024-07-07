#include "pch.h"
#include "Resource.h"
#include "DeviceManager.h"

Vortex::Resource::Resource()
{
	//D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	//cbvHeapDesc.NumDescriptors = 1;
	//cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//winrt::check_hresult(m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap)));
    
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
}

Vortex::Resource::~Resource()
{

}

//Vortex::Mesh::Mesh()
//{
//	//auto glbStream = std::make_shared<std::ifstream>("some\\path.glb", std::ios::binary);
//	//auto streamReader = std::make_unique<StreamReader>();
//	//Microsoft::glTF::GLBResourceReader reader(std::move(streamReader), glbStream);
//
//}
//
//Vortex::Mesh::~Mesh()
//{
//	//m_Resource->Release();
//}
