#include "Fence.h"
#include "Exception.h"

UINT64 Fence::GetValue()
{
    return (m_pFence ? m_pFence->GetCompletedValue() : UINT64_MAX);
}

void Fence::Signal(UINT64 value)
{
    ERROR_IF_NULL_AND_THROW(m_pFence, "Fence was nullptr");
    hr = m_pFence->Signal(value);
    ERROR_IF_FAILED(hr);
}


Fence::WaitObject Fence::CreateWaitObject(UINT64 value)
{
    ERROR_IF_NULL_AND_THROW(m_pFence, "Pointer was nullptr");
    return WaitObject(*this, value);
}

Fence::operator UINT64()
{
    return GetValue();
}

Fence::operator ID3D12Fence* ()
{
    return m_pFence.Get();
}

ID3D12Fence** Fence::operator&()
{
    return &m_pFence;
}

ID3D12Fence* Fence::operator->()
{
    return *this;
}

bool Fence::WaitObject::isDone()
{
    return (UINT64)m_fence >= m_value;
}

void Fence::WaitObject::Wait()
{
    ERROR_IF_NULL_AND_RETURN(m_fence.GetValue() != UINT64_MAX, "Invalid fence value");

    while (!isDone())
    {
        _mm_pause();
        ERROR_IF_NULL_AND_RETURN(m_fence.GetValue() != UINT64_MAX, "Invalid fence value");
    }
}

Fence::Fence(ID3D12Device* pDevice)
{
    hr = pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
    ERROR_IF_FAILED(hr);
}

Fence::WaitObject::WaitObject(Fence& rFence, UINT64 value):
    m_fence(rFence),
    m_value(value)
{
    ERROR_IF_NULL_AND_RETURN(
        m_fence.GetValue() != UINT64_MAX,
        "Invalid fence"
    );

}

Fence::WaitObject::operator bool()
{
    return isDone();
}
