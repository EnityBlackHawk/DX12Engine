#include "SwapChain.h"
#include "Exception.h"

SwapChain::~SwapChain()
{
    //Release();
}

void SwapChain::Present(bool vsync)
{
    if (m_pSwapChain)
    {
        //Exception::ErrorMessage("NULL SwapChain", __LINE__, __FILE__);
        return;
    }

    hr = m_pSwapChain->Present(vsync ? 1 : 0, vsync ? NULL : DXGI_PRESENT_ALLOW_TEARING);
    ERROR_IF_FAILED_AND_RETURN(hr);

    m_currentBuffers = (m_currentBuffers + 1) % m_uiUsedBuffers;
}

void SwapChain::Resize(UINT width, UINT height)
{
    dropBuffer();

    hr = m_pSwapChain->ResizeBuffers(m_uiUsedBuffers, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
    ERROR_IF_FAILED_AND_RETURN(hr);
    retrieveBuffer();
}

ID3D12Resource* SwapChain::GetCurrentBuffer()
{
    return m_arrPBuffer[m_currentBuffers].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCurrentRTVHandle()
{
    D3D12_CPU_DESCRIPTOR_HANDLE currentRtvHandle = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();
    currentRtvHandle.ptr += (size_t)m_currentBuffers * m_uiRTVHeapIncrement;

    return currentRtvHandle;
}

UINT SwapChain::NumberOfFramesInFlight() noexcept
{
    return m_uiUsedBuffers;
}

void SwapChain::Release()
{
    dropBuffer();

    if(m_pRtvHeap)
        m_pRtvHeap->Release();
    if(m_pDevice)
        m_pDevice->Release();
    if(m_pSwapChain)
        m_pSwapChain->Release();
}

SwapChain::operator IDXGISwapChain1*()
{
    return m_pSwapChain.Get();
}

IDXGISwapChain1** SwapChain::operator&()
{
    return &m_pSwapChain;
}

IDXGISwapChain1* SwapChain::operator->()
{
    return m_pSwapChain.Get();
}

SwapChain::SwapChain(ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue, IDXGIFactory* pFactory, HWND hWnd, DXGI_FORMAT bufferFormat):
    m_pDevice(pDevice)
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    ZeroMemory(&rtvHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.NumDescriptors = 2; // number of buffers
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = NULL;

    // create heap
    hr = pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pRtvHeap));
    ERROR_IF_FAILED_AND_RETURN(hr);

    // get heap increment size
    m_uiRTVHeapIncrement = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    
    // retrive window client rect
    RECT windowClientRect = {};
    GetClientRect(hWnd, &windowClientRect);

    // CreateSwapChain desc
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    swapChainDesc.Width = windowClientRect.right - windowClientRect.left;
    swapChainDesc.Height = windowClientRect.bottom - windowClientRect.top;
    swapChainDesc.Format = bufferFormat;
    swapChainDesc.Stereo = false; // no 3D
    swapChainDesc.SampleDesc = { 1, 0 }; // No MSAA;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2; // number of buffers
    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    // Create swap chain fullscreen mode descriptor
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFDesc = { 0 };
    swapChainFDesc.Windowed = true;

    ComPtr<IDXGIFactory2> pFactory2;

    hr = pFactory->QueryInterface(IID_PPV_ARGS(&pFactory2));
    
    ERROR_IF_FAILED_AND_RETURN(hr);

    // Create swap chain
    hr = pFactory2->CreateSwapChainForHwnd(
        pCommandQueue,
        hWnd,
        &swapChainDesc,
        &swapChainFDesc,
        nullptr,
        &m_pSwapChain
    );

    ERROR_IF_FAILED_AND_RETURN(hr);

    m_uiUsedBuffers = 2;

    retrieveBuffer();

}

void SwapChain::dropBuffer()
{
    for (UINT i = 0; i < m_uiUsedBuffers; i++)
        m_arrPBuffer[i]->Release();
}

void SwapChain::retrieveBuffer()
{
    for (UINT i = 0; i < m_uiUsedBuffers; i++)
    {
        if(m_arrPBuffer[i].Get())
            m_arrPBuffer[i]->Release();
        hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&(m_arrPBuffer[i])));
        ERROR_IF_FAILED(hr);

        // Get heap buffer handle
        D3D12_CPU_DESCRIPTOR_HANDLE heapHandleCpu = m_pRtvHeap->GetCPUDescriptorHandleForHeapStart();
        heapHandleCpu.ptr += (size_t)m_uiRTVHeapIncrement * i;

        // create RTV
        m_pDevice->CreateRenderTargetView(m_arrPBuffer->Get(), NULL, heapHandleCpu);
    }
}
