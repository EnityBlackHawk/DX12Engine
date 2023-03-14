#pragma once
#include "Fence.h"
#include <atomic>

class FenceCounter
{
public:
	class Frontend;

	FenceCounter(Fence& rFence);
	void Release();
	UINT64 Head();
	UINT64 Next();
	void SetFenceObject(Fence& rFence);
	FenceCounter::Frontend newFrontEnd();


	void operator=(Fence& other);
	operator UINT64();
	operator bool();

	FenceCounter(const FenceCounter&) = delete;
	void operator =(const FenceCounter&) = delete;

protected:

	Fence m_fence;
	std::atomic<UINT64> m_counter = 0;
	std::atomic_flag m_lock = ATOMIC_FLAG_INIT;

public:

	class Frontend
	{
	public:
		Frontend() = default;
		~Frontend();
		Frontend(FenceCounter& rFenceCounter);
		void Release();
		UINT64 next();
		UINT64 head();
		Fence& GetFence();
		Fence::WaitObject GetCurrentWaitObject();
		
		operator bool();
		operator Fence&();
		operator ID3D12Fence* ();

		void operator= (FenceCounter& other);

	private:

		FenceCounter* m_pFenceCounter = nullptr;
		UINT64 m_head = 0;
	};

};

