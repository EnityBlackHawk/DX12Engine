#include "CommandQueueManager.h"
#include "Exception.h"

CommandQueueManager CommandQueueManager::s_instance;

void CommandQueueManager::createInternalObjects(ID3D12Device* pDevice)
{
    ERROR_IF_NULL_AND_RETURN(pDevice, "Device was null ptr");
    destroyInternalObjects();

    // create all queues
    for (int i = 0; i < 3; i++)
    {
        CommandQueue queue(pDevice, arrCommandQueueTypes[i]);
        m_arrCommandQueues[i] = queue;
    }

}

void CommandQueueManager::destroyInternalObjects()
{
    for (int i = 0; i < 3; i++)
    {
        if(m_arrCommandQueues[i])
            m_arrCommandQueues[i]->Release();
    }
}

CommandQueue& CommandQueueManager::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type)
{
    int idx = -1;
    for (int i = 0; i < 3; i++)
        if (arrCommandQueueTypes[i] == type)
            idx = i;

    ERROR_IF_NULL_AND_THROW(idx >= 0, "No command queue for given command list type");
    try {
        m_arrCommandQueues[idx];
    }
    catch (std::exception e)
    {
        ERROR_MESSAGE_AND_THROW("Command queue is not valid");
    }

    return m_arrCommandQueues[idx];
}

CommandQueueManager& CommandQueueManager::GetInstance()
{
    return s_instance;
}
