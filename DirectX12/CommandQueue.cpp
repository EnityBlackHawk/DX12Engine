#include "CommandQueue.h"
#include "Exception.h"

CommandQueue::operator ID3D12CommandQueue* ()
{
	return m_pCommandQueue.Get();
}

ID3D12CommandQueue** CommandQueue::operator&()
{
	return &m_pCommandQueue;
}

ID3D12CommandQueue* CommandQueue::operator->()
{
	return *this;
}

CommandQueue::CommandQueue(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE type):
	m_type(type)
{
	if (!pDevice)
	{
		Exception::ErrorMessage("pDevice is NULL", __LINE__, __FILE__);
		return;
	}

	D3D12_COMMAND_QUEUE_DESC qd;
	ZeroMemory(&qd, sizeof(qd));

	qd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	qd.Type = type;
	qd.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	qd.NodeMask = NULL;

	hr = pDevice->CreateCommandQueue(&qd, IID_PPV_ARGS(&m_pCommandQueue));
	ERROR_IF_FAILED(hr);

}
