#pragma once

namespace Vortex
{
	class VORTEX_API Renderer
	{
	public:
		Renderer() = delete;
		Renderer(const Renderer&) = delete;
		Renderer(HWND hwnd, UINT width, UINT height);
		~Renderer();

		void Render();


		void PopulateCommandList();

		void WaitForPreviousFrame();

		winrt::com_ptr<IDXGISwapChain3> GetSwapChain() { return m_swapChain; }
	private:

		static const UINT FrameCount = 2;

		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT4 color;
		};

		winrt::com_ptr<IDCompositionDevice> dcompDevice;
		winrt::com_ptr<IDCompositionTarget> dcompTarget;
		winrt::com_ptr<IDCompositionVisual> dcompVisual;

		// Pipeline objects.
		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;
		winrt::com_ptr<IDXGISwapChain3> m_swapChain;
		winrt::com_ptr<ID3D12Device2> m_device;
		winrt::com_ptr<ID3D12Resource> m_renderTargets[FrameCount];
		winrt::com_ptr<ID3D12CommandAllocator> m_commandAllocator;
		winrt::com_ptr<ID3D12CommandQueue> m_commandQueue;
		winrt::com_ptr<ID3D12RootSignature> m_rootSignature;
		winrt::com_ptr<ID3D12DescriptorHeap> m_rtvHeap;
		winrt::com_ptr<ID3D12PipelineState> m_pipelineState;
		winrt::com_ptr<ID3D12GraphicsCommandList6> m_commandList;
		UINT m_rtvDescriptorSize;

		// App resources.
		winrt::com_ptr<ID3D12Resource> m_vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;


		// Synchronization objects.
		UINT m_frameIndex;
		HANDLE m_fenceEvent;
		winrt::com_ptr<ID3D12Fence> m_fence;
		UINT64 m_fenceValue;

		// Viewport dimensions.
		UINT m_width;
		UINT m_height;


		inline HRESULT ReadDataFromFile(LPCWSTR filename, BYTE** data, UINT* size)
		{
			CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {};
			extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
			extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
			extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
			extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
			extendedParams.lpSecurityAttributes = nullptr;
			extendedParams.hTemplateFile = nullptr;

			winrt::file_handle file(CreateFile2(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &extendedParams));
			if (file.get() == INVALID_HANDLE_VALUE)
			{
				throw std::exception();
			}

			FILE_STANDARD_INFO fileInfo = {};
			if (!GetFileInformationByHandleEx(file.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
			{
				throw std::exception();
			}

			if (fileInfo.EndOfFile.HighPart != 0)
			{
				throw std::exception();
			}

			*data = reinterpret_cast<byte*>(malloc(fileInfo.EndOfFile.LowPart));
			*size = fileInfo.EndOfFile.LowPart;

			if (!ReadFile(file.get(), *data, fileInfo.EndOfFile.LowPart, nullptr, nullptr))
			{
				throw std::exception();
			}

			return S_OK;
		}
	};
}