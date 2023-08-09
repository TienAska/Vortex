#include "pch.h"

#include "Shader.h"

Vortex::Shader::Shader(const winrt::hstring& filename, const winrt::hstring& entryPoint, Type type) : m_type(type)
{
	// Create DXC objects
	winrt::com_ptr<IDxcUtils> dxcUtils;
	winrt::check_hresult(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxcUtils.put())));
	winrt::com_ptr<IDxcCompiler3> dxcCompiler;
	winrt::check_hresult(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxcCompiler.put())));

	// Load file
	winrt::com_ptr<IDxcBlobEncoding> dxcBlobEncoding;
	winrt::check_hresult(dxcUtils->LoadFile(filename.c_str(), nullptr, dxcBlobEncoding.put()));
	DxcBuffer dxcBuffer = {
		dxcBlobEncoding->GetBufferPointer(),
		dxcBlobEncoding->GetBufferSize(),
		DXC_CP_ACP
	};

	std::wstring binaryName(filename);
	binaryName += L".bin";

	// Build arguments
	LPCWSTR args[]{
		L"-Fo", binaryName.c_str(),
#if defined(_DEBUG)
			DXC_ARG_DEBUG,
			DXC_ARG_SKIP_OPTIMIZATIONS,
#endif
	};
	LPCWSTR target = nullptr;
	switch (type)
	{
	case Shader::Vertex:
		target = L"vs_6_5";
		break;
	case Shader::Amplification:
		target = L"as_6_5";
		break;
	case Shader::Mesh:
		target = L"ms_6_5";
		break;
	case Shader::Pixel:
		target = L"ps_6_5";
		break;
	}
	winrt::com_ptr<IDxcCompilerArgs> compilerArgs;
	winrt::check_hresult(dxcUtils->BuildArguments(
		filename.c_str(),
		entryPoint.c_str(),
		target, args, _countof(args),
		nullptr, 0,
		compilerArgs.put())
	);

	// Compile
	winrt::com_ptr<IDxcResult> dxcResult;
	winrt::check_hresult(dxcCompiler->Compile(
		&dxcBuffer,
		compilerArgs->GetArguments(), compilerArgs->GetCount(),
		nullptr,
		IID_PPV_ARGS(dxcResult.put()))
	);
	winrt::check_hresult(dxcResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(m_dxcBinary.put()), m_dxcBinaryFilename.put()));
}

D3D12_SHADER_BYTECODE Vortex::Shader::GetBytecode() const
{
	return CD3DX12_SHADER_BYTECODE(reinterpret_cast<ID3DBlob*>(m_dxcBinary.get()));
}

winrt::hstring Vortex::Shader::GetBinaryFilename() const
{
	return m_dxcBinaryFilename->GetStringPointer();
}
