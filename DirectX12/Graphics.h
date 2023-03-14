#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

#include "Exception.h"


#define MEM_KiB(kib)		   (kib * 1024)
#define MEM_MiB(mib)	MEM_KiB(mib * 1024)
#define MEM_GiB(gib)	MEM_MiB(gib * 1024)

using namespace Microsoft::WRL;

class Graphics
{
public:
	Graphics();
	~Graphics();
	bool GetAdapterDesc3(DXGI_ADAPTER_DESC3* pDesc) noexcept;
	bool GetVideoMemory(DXGI_QUERY_VIDEO_MEMORY_INFO* pMemory) noexcept;
	bool ReserveVideoMemory(UINT64 reservationBytes);
	void FindMonitor(HMONITOR hMonitor);
	void GetOutputDesc(DXGI_OUTPUT_DESC* pDesc) noexcept;
	
	void DEBUG_SetGPUBasedValidation(bool enable, D3D12_GPU_BASED_VALIDATION_FLAGS flags);
	void SetDebugDeviceFeatureMask(D3D12_DEBUG_FEATURE featureMask);
	void GetDebugDeviceFeatureMask();

	void SetGpuValidatorInfo(D3D12_DEBUG_DEVICE_GPU_BASED_VALIDATION_SETTINGS* pInfo);
	float GetDebugSlowdownFactor();

	UINT64 GetInfoQueueInfoCount();
	size_t GetInfoQueueMessage(UINT64 index, D3D12_MESSAGE* pMessage);

	DWORD RegisterInfoQueueCallback(D3D12MessageFunc messageFunc, void* functionData, D3D12_MESSAGE_CALLBACK_FLAGS flags);
	void UnregisterInfoQueueCallback(DWORD cookie);


#pragma region TesteOnly
	ID3D12Device* GetDevice()
	{
		return m_device.Get();
	}

	IDXGIFactory* GetFactory()
	{
		return m_factory.Get();
	}

	void teste(IDXGISwapChain1* swap)
	{
		return;
	}
#pragma endregion


	template<typename T>
	void GetFeatureSupportD3D12(D3D12_FEATURE feature, T& data)
	{
		if (m_device)
		{
			hr = m_device->CheckFeatureSupport(feature, &data, sizeof(T));
			ERROR_IF_FAILED(hr);
		}
	}

private:
	void getAdapter(LUID preferedAdapter = {});

	void enableDebugInterface();
	
	
	void createDevice();
	void createDebugDevice();
	void createInfoQueue();

	ComPtr<IDXGIFactory1> m_factory;
	ComPtr<IDXGIAdapter> m_adapter;
	ComPtr<IDXGIOutput>  m_output;

	ComPtr<ID3D12Device> m_device;

	ComPtr<ID3D12InfoQueue> m_infoQueue;

	// Debug:
	ComPtr<ID3D12Debug> m_debugLayer;
	ComPtr<ID3D12DebugDevice> m_debugDevice;

	HRESULT hr = S_OK;

};

