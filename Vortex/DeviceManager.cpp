#include "pch.h"

#include "DeviceManager.h"

Vortex::DeviceManager::DeviceManager()
{
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	{
		winrt::com_ptr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	winrt::com_ptr<IDXGIFactory6> factory;
	winrt::check_hresult(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
	m_factory = factory;
	winrt::com_ptr<IDXGIAdapter> hardwareAdapter;

	if (SUCCEEDED(m_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&hardwareAdapter))))
	{
		winrt::check_hresult(D3D12CreateDevice(hardwareAdapter.get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device)));
	}

}
