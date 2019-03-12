#pragma once

namespace xtest{
namespace mesh {
	

	struct PlaneInfo
	{
		unsigned xEdgeVertexCount;
		unsigned zEdgeVertexCount;
		float xEdgeLength;
		float zEdgeLength;
		uint32 vertexCount;
		uint32 indexCount;
	};

	struct CylinderInfo
	{
		unsigned sliceCount;
		unsigned stackCount;
		float height;
		float topRadius;
		float bottomRadius;
		uint32 vertexCount;
		uint32 indexCount;
	};



	PlaneInfo GeneratePlaneInfo(unsigned xEdgeVertexCount, unsigned zEdgeVertexCount, float xEdgeLength, float zEdgeLength);

	void GeneratePlane(
		const PlaneInfo& planeInfo, 
		DirectX::XMFLOAT3* out_positions,
		size_t positionStride,
		DirectX::XMFLOAT3* out_normals,
		size_t normalStride,
		DirectX::XMFLOAT2* out_uvs,
		size_t uvStride,
		DirectX::XMFLOAT3* out_tangets,
		size_t tangentByteStride,
		uint32* out_indices
		);


	CylinderInfo GenerateCylinderInfo(unsigned sliceCount, unsigned stackCount, float height, float topRadius, float bottomRadius);

	void GenerateCylinder(
		const CylinderInfo& cylinderInfo,
		DirectX::XMFLOAT3* out_positions,
		size_t positionStride,
		DirectX::XMFLOAT3* out_normals,
		size_t normalStride,
		DirectX::XMFLOAT2* out_uvs,
		size_t uvStride,
		DirectX::XMFLOAT3* out_tangets,
		size_t tangentByteStride,
		uint32* out_indices
	);

	
} // xtest
} // mesh

