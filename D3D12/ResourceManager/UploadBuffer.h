#pragma once

#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

#include <DxUtils/d3dx12.h>
#include <Utils/DebugUtils.h>

class UploadBuffer {
public:
	UploadBuffer(ID3D12Device& device, const std::size_t elemSize, const std::uint32_t elemCount);

	UploadBuffer(const UploadBuffer& rhs) = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
	~UploadBuffer();

	__forceinline ID3D12Resource* Resource() const noexcept { return mBuffer.Get(); }

	void CopyData(const std::uint32_t elemIndex, void* srcData, const std::size_t srcDataSize) noexcept;

	static std::size_t CalcConstantBufferByteSize(const std::size_t byteSize);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mBuffer;
	std ::uint8_t* mMappedData{ nullptr };
	std::size_t mElemSize{ 0U };
};