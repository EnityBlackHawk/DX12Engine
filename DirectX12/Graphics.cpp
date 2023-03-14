#include "Graphics.h"


Graphics::Graphics()
{
#if defined(_DEBUG)
	enableDebugInterface();
#endif

	hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_factory));
	ERROR_IF_FAILED_AND_EXIT(hr);
	getAdapter();
	if (!m_adapter.Get())
		return;
	createDevice();
	createInfoQueue();
	createDebugDevice();
}

Graphics::~Graphics()
{/*
	m_infoQueue->Release();
	m_device->Release();
	m_output->Release();
	m_adapter->Release();
	m_factory->Release();*/
}

bool Graphics::GetAdapterDesc3(DXGI_ADAPTER_DESC3* pDesc) noexcept
{
	ComPtr<IDXGIAdapter4> pAdapter4;
	hr = m_adapter->QueryInterface(IID_PPV_ARGS(&pAdapter4));
	ERROR_IF_FAILED(hr);
	if (hr == S_OK)
	{
		hr = pAdapter4->GetDesc3(pDesc);
		ERROR_IF_FAILED(hr);
		return SUCCEEDED(hr) ? true : false;
	}
}

bool Graphics::GetVideoMemory(DXGI_QUERY_VIDEO_MEMORY_INFO* pMemory) noexcept
{
	ComPtr<IDXGIAdapter3> pAdapter3;
	hr = m_adapter->QueryInterface(IID_PPV_ARGS(&pAdapter3));
	ERROR_IF_FAILED(hr);
	if (hr == S_OK)
	{
		hr = pAdapter3->QueryVideoMemoryInfo(NULL, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, pMemory);
		ERROR_IF_FAILED(hr);
		return SUCCEEDED(hr) ? true : false;
	}
}

bool Graphics::ReserveVideoMemory(UINT64 reservationBytes)
{
	ComPtr<IDXGIAdapter4> pAdapter4;
	hr = m_adapter->QueryInterface(IID_PPV_ARGS(&pAdapter4));
	ERROR_IF_FAILED(hr);
	if (hr == S_OK)
	{
		hr = pAdapter4->SetVideoMemoryReservation(NULL, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, reservationBytes);
		ERROR_IF_FAILED(hr);
		return SUCCEEDED(hr) ? true : false;
	}
}

void Graphics::FindMonitor(HMONITOR hMonitor)
{

	//Enum outputs
	UINT index = 0;
	while (SUCCEEDED(m_adapter->EnumOutputs(index, &m_output)))
	{
		DXGI_OUTPUT_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_OUTPUT_DESC));

		// Get Desc;
		hr = m_output->GetDesc(&desc);
		ERROR_IF_FAILED_AND_RETURN(hr);

		if (desc.Monitor == hMonitor)
		{
			break;
		}

		m_output->Release();
		index++;
	}
}

void Graphics::GetOutputDesc(DXGI_OUTPUT_DESC* pDesc) noexcept
{
	if (!m_output.Get())
	{
		return;
	}
	ComPtr<IDXGIOutput6> pOutput6;
	hr = m_output->QueryInterface(IID_PPV_ARGS(&pOutput6));
	ERROR_IF_FAILED_AND_RETURN(hr);
	hr = pOutput6->GetDesc(pDesc);
	ERROR_IF_FAILED(hr);
	pOutput6->Release();
}

void Graphics::getAdapter(LUID preferredAdapter)
{
	LUID nullLuid = {};
	if (memcpy(&nullLuid, &preferredAdapter, sizeof(LUID)) != 0)
	{
		ComPtr<IDXGIFactory4> pFactory4;
		hr = m_factory->QueryInterface(IID_PPV_ARGS(&pFactory4));
		ERROR_IF_FAILED(hr);
		if (hr == S_OK)
		{
			hr = pFactory4->EnumAdapterByLuid(preferredAdapter, IID_PPV_ARGS(&m_adapter));
			switch (hr)
			{
			case S_OK:
				return;

			case DXGI_ERROR_NOT_FOUND:
				break;

			default:
				ERROR_MESSAGE(hr);
				return;
			}
		}
	}


	// Find performant adapter
	ComPtr<IDXGIFactory6> pFactory6;
	hr = m_factory->QueryInterface(IID_PPV_ARGS(&pFactory6));
	ERROR_IF_FAILED(hr);

	if (hr == S_OK)
	{
		hr = pFactory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_adapter));
		switch (hr)
		{
		case S_OK:
			return;

		case DXGI_ERROR_NOT_FOUND:
			break;

		default:
			ERROR_MESSAGE(hr);
			break;
		}
	}

	//return default adapter:
	m_factory->EnumAdapters(0, &m_adapter);
	return;
}

void Graphics::DEBUG_SetGPUBasedValidation(bool enable, D3D12_GPU_BASED_VALIDATION_FLAGS flags)
{
	ComPtr<ID3D12Debug3> pDebug3;
	hr = m_debugLayer->QueryInterface(IID_PPV_ARGS(&pDebug3));
	ERROR_IF_FAILED_AND_RETURN(hr);
	
	pDebug3->SetEnableGPUBasedValidation(enable);
	pDebug3->SetGPUBasedValidationFlags(flags);

}

void Graphics::SetDebugDeviceFeatureMask(D3D12_DEBUG_FEATURE featureMask)
{
	ComPtr<ID3D12DebugDevice1> pDDevice1;
	hr = m_debugDevice->QueryInterface(IID_PPV_ARGS(&pDDevice1));
	ERROR_IF_FAILED_AND_RETURN(hr);

	hr = pDDevice1->SetDebugParameter(D3D12_DEBUG_DEVICE_PARAMETER_FEATURE_FLAGS, &featureMask, sizeof(D3D12_DEBUG_FEATURE));
	ERROR_IF_FAILED(hr);
}

void Graphics::GetDebugDeviceFeatureMask()
{
	D3D12_DEBUG_FEATURE featureMask = {};
	ComPtr<ID3D12DebugDevice1> pDDevice1;
	hr = m_debugDevice->QueryInterface(IID_PPV_ARGS(&pDDevice1));
	ERROR_IF_FAILED_AND_RETURN(hr);

	hr = pDDevice1->GetDebugParameter(D3D12_DEBUG_DEVICE_PARAMETER_FEATURE_FLAGS, &featureMask, sizeof(D3D12_DEBUG_FEATURE));
	ERROR_IF_FAILED(hr);
}

void Graphics::SetGpuValidatorInfo(D3D12_DEBUG_DEVICE_GPU_BASED_VALIDATION_SETTINGS* pInfo)
{
	ComPtr<ID3D12DebugDevice1> pDDevice1;
	hr = m_debugDevice->QueryInterface(IID_PPV_ARGS(&pDDevice1));
	ERROR_IF_FAILED_AND_RETURN(hr);

	pDDevice1->SetDebugParameter(D3D12_DEBUG_DEVICE_PARAMETER_GPU_BASED_VALIDATION_SETTINGS, pInfo, sizeof(D3D12_DEBUG_DEVICE_GPU_BASED_VALIDATION_SETTINGS));
	ERROR_IF_FAILED(hr);
}

float Graphics::GetDebugSlowdownFactor()
{
	ComPtr<ID3D12DebugDevice1> pDDevice1;
	hr = m_debugDevice->QueryInterface(IID_PPV_ARGS(&pDDevice1));
	ERROR_IF_FAILED_AND_RETURN_VALUE(hr, 0);

	D3D12_DEBUG_DEVICE_GPU_SLOWDOWN_PERFORMANCE_FACTOR qData;
	qData.SlowdownFactor = 0.0f;

	hr = pDDevice1->GetDebugParameter(D3D12_DEBUG_DEVICE_PARAMETER_GPU_SLOWDOWN_PERFORMANCE_FACTOR, &qData, sizeof(qData));
	ERROR_IF_FAILED(hr);

	return SUCCEEDED(hr) ? qData.SlowdownFactor : 0.0f;

}

UINT64 Graphics::GetInfoQueueInfoCount()
{
	return m_infoQueue.Get() ? m_infoQueue->GetNumStoredMessages() : 0;
}

size_t Graphics::GetInfoQueueMessage(UINT64 index, D3D12_MESSAGE* pMessage)
{
	size_t messageLength = 0;
	if (m_infoQueue.Get())
		m_infoQueue->GetMessage(index, pMessage, &messageLength);
	
	return messageLength;
}

DWORD Graphics::RegisterInfoQueueCallback(D3D12MessageFunc messageFunc, void* functionData, D3D12_MESSAGE_CALLBACK_FLAGS flags)
{
	ComPtr<ID3D12InfoQueue1> pInfo1;
	hr = m_infoQueue->QueryInterface(IID_PPV_ARGS(&pInfo1));
	ERROR_IF_FAILED_AND_RETURN_VALUE(hr, 0);

	DWORD cookie = 0;
	hr = pInfo1->RegisterMessageCallback(messageFunc, flags, functionData, &cookie);
	ERROR_IF_FAILED_AND_RETURN_VALUE(hr, 0);

	return cookie;
}

void Graphics::UnregisterInfoQueueCallback(DWORD cookie)
{
	ComPtr<ID3D12InfoQueue1> pInfo1;
	hr = m_infoQueue->QueryInterface(IID_PPV_ARGS(&pInfo1));
	ERROR_IF_FAILED_AND_RETURN(hr);

	pInfo1->UnregisterMessageCallback(cookie);
}

void Graphics::enableDebugInterface()
{
	D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugLayer));
	m_debugLayer->EnableDebugLayer();
}

void Graphics::createDebugDevice()
{
	hr = m_device->QueryInterface(IID_PPV_ARGS(&m_debugDevice));
	ERROR_IF_FAILED(hr);
}

void Graphics::createInfoQueue()
{
	hr = m_device->QueryInterface(IID_PPV_ARGS(&m_infoQueue));
	ERROR_IF_FAILED(hr);
}


void Graphics::createDevice()
{
	hr = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device));
	ERROR_IF_FAILED(hr);

}


