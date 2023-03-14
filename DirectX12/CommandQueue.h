#pragma once
#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class CommandQueue
{

public:

	operator ID3D12CommandQueue* ();
	ID3D12CommandQueue** operator&();
	ID3D12CommandQueue* operator->();

private:

	friend class Graphics;
	friend class CommandQueueManager;


	CommandQueue() = default;
	CommandQueue(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type);

	ComPtr<ID3D12CommandQueue> m_pCommandQueue;
	D3D12_COMMAND_LIST_TYPE m_type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HRESULT hr = S_OK;
};

