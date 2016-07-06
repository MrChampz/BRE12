#pragma once

#include <d3d12.h>
#include <memory>
#include <tbb/task.h>
#include <vector>

#include <App/CommandListProcessor.h>
#include <RenderTask\CmdBuilderTask.h>
#include <RenderTask\InitTask.h>
#include <Timer/Timer.h>

// It has the responsibility to construct command builders and also execute them (to generate/execute
// command lists in the queue provided by CommandListProcessor)
// Steps:
// - Use MasterRenderTask::Create() to create an instance. You should
//   spawn it using the returned parent tbb::task.
// - When you spawn it, execute() method is automatically called. You should call to Init() and InitCmdBuilders() before spawning.
// - When you want to terminate this task, you should call MasterRenderTask::Terminate() 
//   and wait for termination using parent task.
class MasterRenderTask : public tbb::task {
public:
	static tbb::empty_task* Create(MasterRenderTask* &masterRenderTask);

	MasterRenderTask() = default;

	// You should fill data before calling InitCmdBuilders
	__forceinline std::vector<std::unique_ptr<InitTask>>& GetInitTasks() noexcept { return mInitTasks; }
	__forceinline std::vector<std::unique_ptr<CmdBuilderTask>>& GetCmdBuilderTasks() noexcept { return mCmdBuilderTasks; }

	// Execute in order before spawning it.
	void Init(const HWND hwnd) noexcept;
	void InitCmdBuilders() noexcept;
	
	void Terminate() noexcept { mTerminate = true; }

	// Called when spawned
	tbb::task* execute() override;

private:
	void InitSystems() noexcept;
	
	void ExecuteCmdBuilderTasks() noexcept;
	void Finalize() noexcept;

	void CreateRtvAndDsvDescriptorHeaps() noexcept;
	void CreateRtvAndDsv() noexcept;
	void CreateCommandObjects() noexcept;

	// Used to display milliseconds per frame in window caption (if windowed)
	void CalculateFrameStats() noexcept;

	ID3D12Resource* CurrentBackBuffer() const noexcept;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const noexcept;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const noexcept;

	void FlushCommandQueue() noexcept;
	void SignalFenceAndPresent() noexcept;

	HWND mHwnd{ 0 };

	Timer mTimer;
		
	tbb::empty_task* mCmdListProcessorParent{ nullptr };
	CommandListProcessor* mCmdListProcessor{ nullptr };

	ID3D12CommandQueue* mCmdQueue{ nullptr };
	ID3D12Fence* mFence{ nullptr };
	std::uint64_t mFenceByFrameIndex[Settings::sSwapChainBufferCount]{ 0UL };
	std::uint64_t mCurrentFence{ 0UL };

	// We have 2 commands lists (for frame begin and frame end), and 2
	// commands allocator per list
	ID3D12CommandAllocator* mCmdAllocFrameBegin[Settings::sSwapChainBufferCount]{ nullptr };
	ID3D12CommandAllocator* mCmdAllocFrameEnd[Settings::sSwapChainBufferCount]{ nullptr };
	ID3D12GraphicsCommandList* mCmdListFrameBegin{ nullptr };
	ID3D12GraphicsCommandList* mCmdListFrameEnd{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[Settings::sSwapChainBufferCount];
	ID3D12Resource* mDepthStencilBuffer{ nullptr };

	ID3D12DescriptorHeap* mRtvHeap{ nullptr };
	ID3D12DescriptorHeap* mDsvHeap{ nullptr };

	std::vector<std::unique_ptr<InitTask>> mInitTasks;
	std::vector<std::unique_ptr<CmdBuilderTask>> mCmdBuilderTasks;
	
	bool mTerminate{ false };
};
