#pragma once

#include "Device.h"

namespace Vortex
{
	class SwapChain
	{
	public:
		SwapChain() = delete;
		SwapChain(const SwapChain&) = delete;
		SwapChain(SwapChain&&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;
		SwapChain& operator=(SwapChain&&) = delete;
		SwapChain(const winrt::com_ptr<ID3D12CommandQueue>& commandQueue, HWND hwnd, uint32_t width, uint32_t height);

		inline const CD3DX12_VIEWPORT* GetViewport() const { return &m_viewport; }
		inline const CD3DX12_RECT* GetScissorRect() const { return &m_scissorRect; }

		winrt::com_ptr<ID3D12Resource> GetBackBufferRenderTarget() const;
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetBackBufferRTVHandle() const;
		void Flip();
	private:
        // Window relatives.
        CD3DX12_VIEWPORT m_viewport;
        CD3DX12_RECT m_scissorRect;
		// Render targets
        winrt::com_ptr<IDXGISwapChain3> m_swapChain;
        winrt::com_ptr<ID3D12Resource> m_renderTargets[VX_DOUBLE_BUFFER];
		winrt::com_ptr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
		uint32_t m_rtvDescriptorSize;
	};

	class Renderer
	{
	public:
		class IRenderPass
		{
		public:
			virtual ~IRenderPass() = default;

            inline virtual ID3D12GraphicsCommandList* GetCommandList(std::shared_ptr<SwapChain> swapChain) const = 0;
            inline virtual ID3D12DescriptorHeap* GetDescriptorHeap() const = 0;
		};

		//class RenderPass : public IRenderPass
		//{
		//public:
		//	RenderPass()
		//	{
		//		Setup();
		//		Execution();
		//	}
		//protected:
		//	virtual void Setup();
		//	virtual void Equip() const;
		//	virtual void Execution() const;
		//};

	public:
        Renderer() = delete;
        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;

		Renderer(HWND hwnd, uint32_t width, uint32_t height);
		~Renderer();

		void WaitForPreviousFrame();

		template<typename T>
        inline void AddPass() { 
			m_passes.push_back(std::make_unique<T>()); 
		}

		//void Update();
		void Execute();
		//void PopulateCommandList();


	private:
		// Synchronization objects.
		winrt::handle m_fenceEvent;
        uint64_t m_fenceValue;
        winrt::com_ptr<ID3D12Fence1> m_fence;


		winrt::com_ptr<ID3D12CommandQueue> m_commandQueue;
		winrt::com_ptr<ID3D12CommandAllocator> m_commandAllocator;
		winrt::com_ptr<ID3D12GraphicsCommandList6> m_commandListBegin;
		winrt::com_ptr<ID3D12GraphicsCommandList6> m_commandListEnd;

		std::shared_ptr<SwapChain> m_swapChain;

		std::vector<std::unique_ptr<IRenderPass>> m_passes;

		// Viewport dimensions.
		//uint32_t m_width;
		//uint32_t m_height;

		// Input
		//winrt::com_ptr<IGameInput> m_gameInput;
		//winrt::com_ptr<IGameInputDevice> m_gameDevice;
	};
}