#pragma once

namespace Vortex
{
	class VORTEX_API Shader
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
		winrt::hstring m_sourceFilename;
		winrt::hstring m_cacheFilename;
		winrt::hstring m_entryPoint;
		Type m_type;

		void LoadBinary();
		void Compile();

		winrt::com_ptr<IDxcBlobEncoding> m_cacheBinary;
	};
}