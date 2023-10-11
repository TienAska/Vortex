#include "pch.h"
#include "Resource.h"

//Vortex::Resource::Resource()
//{
//	//D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
//	//cbvHeapDesc.NumDescriptors = 1;
//	//cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//	//cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	//winrt::check_hresult(m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap)));
//
//
//}

Vortex::Mesh::Mesh()
{
	auto glbStream = std::make_shared<std::ifstream>("some\\path.glb", std::ios::binary);
	auto streamReader = std::make_unique<StreamReader>();
	Microsoft::glTF::GLBResourceReader reader(std::move(streamReader), glbStream);

}
