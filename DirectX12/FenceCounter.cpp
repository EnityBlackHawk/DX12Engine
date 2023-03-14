#include "FenceCounter.h"
#include "Exception.h"

FenceCounter::FenceCounter(Fence& rFence):
    m_fence(rFence)
{
    if ((UINT64)rFence)
        rFence.Signal(0);
}

void FenceCounter::Release()
{
    while (m_lock.test_and_set(std::memory_order_acquire));

    // Release member
    m_fence->Release();
    m_counter.store(0);

    // unlock object
    m_lock.clear(std::memory_order_acquire);
}

UINT64 FenceCounter::Head()
{
    while (m_lock.test(std::memory_order_acquire));

    return (m_fence ? m_counter.load(std::memory_order_relaxed) : 0);
}

void FenceCounter::SetFenceObject(Fence& rFence)
{
    while (m_lock.test_and_set(std::memory_order_acquire));

    if (&m_fence)
        m_fence.CreateWaitObject(m_counter).Wait();
    

    m_fence = rFence;
    rFence.Signal(m_counter);

    m_lock.clear(std::memory_order_acquire);
}

FenceCounter::Frontend FenceCounter::newFrontEnd()
{
    return Frontend(*this);
}

void FenceCounter::operator=(Fence& other)
{
    SetFenceObject(other);
}

FenceCounter::operator UINT64()
{
    return Head();
}

FenceCounter::operator bool()
{
    return m_fence;
}

UINT64 FenceCounter::Next()
{
    while (m_lock.test(std::memory_order_acquire));
    return ++m_counter;
}

FenceCounter::Frontend::~Frontend()
{
    Release();
}

FenceCounter::Frontend::Frontend(FenceCounter& rFenceCounter):
    m_pFenceCounter(&rFenceCounter)
{
}

void FenceCounter::Frontend::Release()
{
    if (m_pFenceCounter)
    {
        GetCurrentWaitObject().Wait();
    }

    m_pFenceCounter = nullptr;
    m_head = 0;
}

UINT64 FenceCounter::Frontend::next()
{
    ERROR_IF_NULL_AND_THROW(m_pFenceCounter, "Counter pointer was null");

    m_head = m_pFenceCounter->Next();
    return m_head;
}

UINT64 FenceCounter::Frontend::head()
{
    return m_head;
}

Fence& FenceCounter::Frontend::GetFence()
{
    ERROR_IF_NULL_AND_THROW(m_pFenceCounter, "Counter pointer was null");
    return m_pFenceCounter->m_fence;
}

Fence::WaitObject FenceCounter::Frontend::GetCurrentWaitObject()
{
    ERROR_IF_NULL_AND_THROW(m_pFenceCounter, "Value was nullptr");
    return m_pFenceCounter->m_fence.CreateWaitObject(m_head);
}

FenceCounter::Frontend::operator bool()
{
    return m_pFenceCounter;
}

FenceCounter::Frontend::operator Fence& ()
{
    return GetFence();
}

FenceCounter::Frontend::operator ID3D12Fence* ()
{
    return m_pFenceCounter->m_fence;
}

void FenceCounter::Frontend::operator=(FenceCounter& other)
{
    if (m_pFenceCounter)
    {
        GetCurrentWaitObject().Wait();
    }

    m_pFenceCounter = &other;
    m_head = 0;
}
