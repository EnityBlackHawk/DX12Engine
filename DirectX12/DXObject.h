#pragma once
#include <Windows.h>
#include "d3dx12.h"
#include <DirectXMath.h>
#include <dxgi1_6.h>
#include <wrl.h>


using namespace Microsoft::WRL;

class DXObject
{

public:

	DXObject(unsigned int width, unsigned int height);

	void OnInit();
	void OnUpdate();
	void OnRender();
	void OnDestroy();

	unsigned int width;
	unsigned int height;
	HWND hwnd;

private:

	static const UINT frameCount = 2;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;


	ComPtr<IDXGISwapChain3> m_swapChain;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12Resource> m_renderTargets[frameCount];
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	UINT m_rtvDescriptorSize;

	// App resources:
	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	
	// Synchronization objects.
	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_fenceValue;

	void LoadPipeline();
	void LoadAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();
	void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = false);

	//
	bool m_useWarpDevice;

};

