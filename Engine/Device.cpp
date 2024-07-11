#include "pch.h"

#include "Device.h"


void Vortex::Device::Initialize()
{
    uint32_t dxgiFactoryFlags = 0;
#if defined(_DEBUG)
    {
        winrt::com_ptr<ID3D12Debug1> debugController;
        winrt::check_hresult(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        debugController->EnableDebugLayer();
        //debugController->SetEnableGPUBasedValidation(true);
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif
    winrt::check_hresult(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&s_dxgiFactory)));

	winrt::com_ptr<IDXGIAdapter3> hardwareAdapter;
    for (uint32_t i = 0; SUCCEEDED(s_dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&hardwareAdapter))); ++i)
    {
        s_deviceList.push_back(hardwareAdapter);
    }
}

winrt::com_ptr<ID3D12Resource> Vortex::Device::CreateResource()
{
    return nullptr;
}

Vortex::Device::Device(const winrt::com_ptr<IDXGIAdapter3>& adaptor)
{
    winrt::check_hresult(adaptor->GetDesc2(&m_adapterDesc));
    D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_12_2;
    if (m_adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) // WARP
        level = D3D_FEATURE_LEVEL_12_1;
    winrt::check_hresult(D3D12CreateDevice(adaptor.get(), level, IID_PPV_ARGS(&m_d3d12Device)));
}

winrt::com_ptr<IDXGIFactory6> Vortex::Device::s_dxgiFactory;

std::vector<Vortex::Device> Vortex::Device::s_deviceList;
