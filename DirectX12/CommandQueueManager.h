#pragma once
#include <d3d12.h>
#include "CommandQueue.h"

class CommandQueueManager
{
public:
	void createInternalObjects(ID3D12Device* pDevice);
	void destroyInternalObjects();
	CommandQueue& GetCommandQueue(D3D12_COMMAND_LIST_TYPE type);

private:

	CommandQueue m_arrCommandQueues[3];

	const D3D12_COMMAND_LIST_TYPE arrCommandQueueTypes[3] =
	{
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		D3D12_COMMAND_LIST_TYPE_COPY,
		D3D12_COMMAND_LIST_TYPE_COMPUTE
	};


public:
	static CommandQueueManager& GetInstance();

	CommandQueueManager(const CommandQueueManager&) = delete;
	void operator=(const CommandQueueManager&) = delete;

private:
	CommandQueueManager() {};

	static CommandQueueManager s_instance;

};

