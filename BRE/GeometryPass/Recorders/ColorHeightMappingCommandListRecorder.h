#pragma once

#include <GeometryPass/GeometryCommandListRecorder.h>
#include <ResourceManager\UploadBuffer.h>

namespace BRE {
class MaterialProperties;

///
/// @brief Responsible to record command lists that implement color height mapping
///
class ColorHeightMappingCommandListRecorder : public GeometryCommandListRecorder {
public:
    ColorHeightMappingCommandListRecorder() = default;
    ~ColorHeightMappingCommandListRecorder() = default;
    ColorHeightMappingCommandListRecorder(const ColorHeightMappingCommandListRecorder&) = delete;
    const ColorHeightMappingCommandListRecorder& operator=(const ColorHeightMappingCommandListRecorder&) = delete;
    ColorHeightMappingCommandListRecorder(ColorHeightMappingCommandListRecorder&&) = default;
    ColorHeightMappingCommandListRecorder& operator=(ColorHeightMappingCommandListRecorder&&) = default;

    ///
    /// @brief Initializes pipeline state object and root signature
    /// @param geometryBufferFormats List of geometry buffers formats. It must be not nullptr
    /// @param geometryBufferCount Number of geometry buffers formats in @p geometryBufferFormats
    ///
    static void InitSharedPSOAndRootSignature(const DXGI_FORMAT* geometryBufferFormats,
                                              const std::uint32_t geometryBufferCount) noexcept;

    ///
    /// @brief Initializes the recorder
    ///
    /// InitSharedPSOAndRootSignature() must be called first
    ///
    /// @param geometryDataVector List of geometry data. Must not be empty
    /// @param materialProperties List of material properties. Must not be empty.
    /// @param normalTextures List of normal textures. Must not be empty.
    /// @param heightTextures List of height textures. Must not be empty.
    ///
    void Init(const std::vector<GeometryData>& geometryDataVector,
              const std::vector<MaterialProperties>& materialProperties,
              const std::vector<ID3D12Resource*>& normalTextures,
              const std::vector<ID3D12Resource*>& heightTextures) noexcept;

    ///
    /// @brief Records and push command lists to CommandListExecutor
    ///
    /// Init() must be called first
    ///
    /// @param frameCBuffer Constant buffer per frame, for current frame
    ///
    void RecordAndPushCommandLists(const FrameCBuffer& frameCBuffer) noexcept final override;

    ///
    /// @brief Checks if internal data is valid. Typically, used for assertions
    /// @return True if valid. Otherwise, false
    ///
    bool IsDataValid() const noexcept final override;

private:
    ///
    /// @brief Initializes the constant buffers
    /// @param materialProperties List of material properties. Must not be empty.
    /// @param normalTextures List of normal textures. Must not be empty.
    /// @param heightTextures List of height textures. Must not be empty.
    ///
    void InitConstantBuffers(const std::vector<MaterialProperties>& materialProperties,
                             const std::vector<ID3D12Resource*>& normalTextures,
                             const std::vector<ID3D12Resource*>& heightTextures) noexcept;

    D3D12_GPU_DESCRIPTOR_HANDLE mNormalBufferGpuDescriptorsBegin;
    D3D12_GPU_DESCRIPTOR_HANDLE mHeightBufferGpuDescriptorsBegin;

    UploadBuffer* mHeightMappingUploadCBuffer{ nullptr };
};
}