#include "RenderTargetDescriptorManager.h"

#include <memory>

#include <DirectXManager\DirectXManager.h>
#include <DXUtils/d3dx12.h>
#include <SettingsManager\SettingsManager.h>

namespace {
	std::unique_ptr<RenderTargetDescriptorManager> gManager{ nullptr };
}

RenderTargetDescriptorManager& RenderTargetDescriptorManager::Create() noexcept {
	ASSERT(gManager == nullptr);
	gManager.reset(new RenderTargetDescriptorManager());
	return *gManager.get();
}

RenderTargetDescriptorManager& RenderTargetDescriptorManager::Get() noexcept {
	ASSERT(gManager != nullptr);
	return *gManager.get();
}

RenderTargetDescriptorManager::RenderTargetDescriptorManager() {
	D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewDescriptorHeapDescriptor{};
	renderTargetViewDescriptorHeapDescriptor.NumDescriptors = 10U;
	renderTargetViewDescriptorHeapDescriptor.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	renderTargetViewDescriptorHeapDescriptor.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	renderTargetViewDescriptorHeapDescriptor.NodeMask = 0;

	mMutex.lock();
	CHECK_HR(DirectXManager::GetDevice().CreateDescriptorHeap(
		&renderTargetViewDescriptorHeapDescriptor,
		IID_PPV_ARGS(mRenderTargetViewDescriptorHeap.GetAddressOf())));
	mMutex.unlock();
	
	mCurrentRenderTargetViewDescriptorHandle = mRenderTargetViewDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	mCurrentRenderTargetViewCpuDescriptorHandle = mRenderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_GPU_DESCRIPTOR_HANDLE RenderTargetDescriptorManager::CreateRenderTargetView(
	ID3D12Resource& resource,
	const D3D12_RENDER_TARGET_VIEW_DESC& descriptor,
	D3D12_CPU_DESCRIPTOR_HANDLE* firstViewCpuDescriptorHandle) noexcept
{
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle{};

	mMutex.lock();
	gpuDescriptorHandle = mCurrentRenderTargetViewDescriptorHandle;

	if (firstViewCpuDescriptorHandle != nullptr) {
		*firstViewCpuDescriptorHandle = mCurrentRenderTargetViewCpuDescriptorHandle;
	}

	DirectXManager::GetDevice().CreateRenderTargetView(&resource, &descriptor, mCurrentRenderTargetViewCpuDescriptorHandle);

	mCurrentRenderTargetViewDescriptorHandle.ptr += DirectXManager::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	mCurrentRenderTargetViewCpuDescriptorHandle.ptr += DirectXManager::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	mMutex.unlock();

	return gpuDescriptorHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE RenderTargetDescriptorManager::CreateRenderTargetViews(
	ID3D12Resource* *resources,
	const D3D12_RENDER_TARGET_VIEW_DESC* descriptors,
	const std::uint32_t descriptorCount,
	D3D12_CPU_DESCRIPTOR_HANDLE* firstViewCpuDescriptorHandle) noexcept
{
	ASSERT(resources != nullptr);
	ASSERT(descriptors != nullptr);
	ASSERT(descriptorCount > 0U);

	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle{};

	mMutex.lock();
	gpuDescriptorHandle = mCurrentRenderTargetViewDescriptorHandle;

	if (firstViewCpuDescriptorHandle != nullptr) {
		*firstViewCpuDescriptorHandle = mCurrentRenderTargetViewCpuDescriptorHandle;
	}

	for (std::uint32_t i = 0U; i < descriptorCount; ++i) {
		ASSERT(resources[i] != nullptr);
		DirectXManager::GetDevice().CreateRenderTargetView(resources[i], &descriptors[i], mCurrentRenderTargetViewCpuDescriptorHandle);
		mCurrentRenderTargetViewCpuDescriptorHandle.ptr += DirectXManager::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	mCurrentRenderTargetViewDescriptorHandle.ptr += descriptorCount * DirectXManager::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	mMutex.unlock();

	return gpuDescriptorHandle;
}