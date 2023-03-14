#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <windef.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class SwapChain
{

	friend class Graphics;

public:
	
	SwapChain(ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue, IDXGIFactory* pFactory, HWND hWnd, DXGI_FORMAT  bufferFormat);
	~SwapChain();
	void Present(bool vsync);
	void Resize(UINT width, UINT height);
	ID3D12Resource* GetCurrentBuffer();
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle();
	UINT NumberOfFramesInFlight() noexcept;
	void Release();

	operator IDXGISwapChain1*();
	IDXGISwapChain1** operator &();
	IDXGISwapChain1* operator ->();


private:

	void dropBuffer();
	void retrieveBuffer();

	ComPtr<IDXGISwapChain1> m_pSwapChain;

	ComPtr<ID3D12Resource> m_arrPBuffer[3] = {};
	ComPtr<ID3D12DescriptorHeap> m_pRtvHeap;
	ComPtr<ID3D12Device> m_pDevice;


	UINT m_uiRTVHeapIncrement = 0;
	UINT m_uiUsedBuffers = 0;
	UINT m_currentBuffers = 0;

	HRESULT hr;


};

