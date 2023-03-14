#pragma once
#include <d3d12.h>
#include <wrl.h>

using namespace Microsoft::WRL;

class Fence
{

public:

	class WaitObject;

	Fence(ID3D12Device* pDevice);

	UINT64 GetValue();
	void Signal(UINT64 value);

	Fence::WaitObject CreateWaitObject(UINT64 value);

	operator UINT64();
	operator ID3D12Fence* ();
	ID3D12Fence** operator &();
	ID3D12Fence* operator ->();
	
	class WaitObject
	{
	public:
		WaitObject(Fence& rFence, UINT64 value);
		bool isDone();
		void Wait();

		operator bool();
		void operator()();

	private:
		Fence& m_fence;
		const UINT64 m_value;
	};

private:
	
	ComPtr<ID3D12Fence> m_pFence;

	HRESULT hr = S_OK;

};

