#pragma once

#include <CommandManager\CommandListPerFrame.h>
#include <ResourceManager\UploadBuffer.h>

namespace BRE {
///
/// @brief Responsible of recording command lists that apply blur
///
class BlurCommandListRecorder {
public:
    BlurCommandListRecorder() = default;
    ~BlurCommandListRecorder() = default;
    BlurCommandListRecorder(const BlurCommandListRecorder&) = delete;
    const BlurCommandListRecorder& operator=(const BlurCommandListRecorder&) = delete;
    BlurCommandListRecorder(BlurCommandListRecorder&&) = default;
    BlurCommandListRecorder& operator=(BlurCommandListRecorder&&) = default;

    ///
    /// @brief Initializes pipeline state object and root signature
    ///
    /// This method must be called at the beginning of the application, and once
    ///
    static void InitSharedPSOAndRootSignature() noexcept;

    ///
    /// @brief Initialize the recorder
    ///
    /// This method must be called after InitSharedPSOAndRootSignature
    ///
    /// @param ambientAccessibilityBufferShaderResourceView Shader resource view to 
    /// the ambient accessibility buffer
    /// @param outputAmbientAccessibilityBufferRenderTargetView Render target view to
    /// the blurred ambient accessibility buffer
    ///
    void Init(const D3D12_GPU_DESCRIPTOR_HANDLE& ambientAccessibilityBufferShaderResourceView,
              const D3D12_CPU_DESCRIPTOR_HANDLE& outputAmbientAccessibilityBufferRenderTargetView) noexcept;

    ///
    /// @brief Records command lists and pushes them into CommandListExecutor
    ///
    /// Init() must be called first
    ///
    /// @return The number of pushed command lists
    ///
    std::uint32_t RecordAndPushCommandLists() noexcept;

    ///
    /// @brief Checks if internal data is valid. Typically, used for assertions
    /// @return True if valid. Otherwise, false
    ///
    bool IsDataValid() const noexcept;

private:
    ///
    /// @brief Initialize blur constant buffer
    ///
    void InitBlurCBuffer() noexcept;

    CommandListPerFrame mCommandListPerFrame;

    D3D12_GPU_DESCRIPTOR_HANDLE mAmbientAccessibilityBufferShaderResourceView{ 0UL };
    D3D12_CPU_DESCRIPTOR_HANDLE mOutputAmbientAccessibilityBufferRenderTargetView{ 0UL };

    UploadBuffer* mBlurUploadCBuffer{ nullptr };
};
}