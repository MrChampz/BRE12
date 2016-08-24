#pragma once

#include <vector>

#include <GeometryGenerator/GeometryGenerator.h>

struct ID3D12GraphicsCommandList;
class Mesh;

class Model {
public:
	explicit Model(const char* filename, ID3D12GraphicsCommandList& cmdList);
	explicit Model(const GeometryGenerator::MeshData& meshData, ID3D12GraphicsCommandList& cmdList);

	Model(const Model& rhs) = delete;
	Model& operator=(const Model& rhs) = delete;
	~Model();

	__forceinline bool HasMeshes() const noexcept { return (mMeshes.size() > 0UL); }
	__forceinline const std::vector<Mesh*>& Meshes() const noexcept { return mMeshes; }

private:
	std::vector<Mesh*> mMeshes;
};