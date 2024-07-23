#pragma once

#include "Assets/Shaders/ShaderUniforms.h"

namespace Vortex
{
	class Shader
	{
	public:
		enum Type
		{
			Vertex,
			Amplification,
			Mesh,
			Pixel,
			Compute
		};

		Shader() = delete;
		Shader(const Shader&) = delete;
		Shader(const winrt::hstring& name);

		static const winrt::hstring s_shaderFolder;
		static const winrt::hstring s_cacheFolder;

		D3D12_SHADER_BYTECODE GetBytecode() const;

	private:
		class IncludeHandler : public IDxcIncludeHandler
		{
			HRESULT STDMETHODCALLTYPE LoadSource(_In_z_ LPCWSTR pFilename, _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource) = 0;
		};

		winrt::hstring m_sourceFilename;
		winrt::hstring m_cacheFilename;
		winrt::hstring m_entryPoint;
		Type m_type;

		void LoadBinary();
		void Preprocess();
		void Compile();

		winrt::com_ptr<IDxcBlobEncoding> m_cacheBinary;
	};
}