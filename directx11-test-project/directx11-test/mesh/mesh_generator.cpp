#include "stdafx.h"
#include "mesh_generator.h"
#include <math/math_utils.h>


using namespace xtest::mesh;
using namespace DirectX;


PlaneInfo xtest::mesh::GeneratePlaneInfo(unsigned xEdgeVertexCount, unsigned zEdgeVertexCount, float xEdgeLength, float zEdgeLength)
{
	XTEST_ASSERT(xEdgeLength > 0.f);
	XTEST_ASSERT(zEdgeLength > 0.f);
	XTEST_ASSERT(xEdgeVertexCount > 1);
	XTEST_ASSERT(zEdgeVertexCount > 1);

	PlaneInfo planeInfo;
	planeInfo.xEdgeVertexCount = xEdgeVertexCount;
	planeInfo.zEdgeVertexCount = zEdgeVertexCount;
	planeInfo.xEdgeLength = xEdgeLength;
	planeInfo.zEdgeLength = zEdgeLength;
	planeInfo.vertexCount = xEdgeVertexCount * zEdgeVertexCount;
	planeInfo.indexCount = (xEdgeVertexCount - 1) * (zEdgeVertexCount - 1) * 6;

	return planeInfo;
}

void xtest::mesh::GeneratePlane(
	const PlaneInfo& planeInfo,
	DirectX::XMFLOAT3* out_positions, 
	size_t positionByteStride, 
	DirectX::XMFLOAT3* out_normals, 
	size_t normalByteStride,
	DirectX::XMFLOAT2* out_uvs, 
	size_t uvByteStride, 
	DirectX::XMFLOAT3* out_tangets,
	size_t tangentByteStride,
	uint32* out_indices)
{

	XTEST_ASSERT(out_positions, L"Vertex positions destination must be specified.");
	XTEST_ASSERT(out_indices, L"Vertex indices destination must be specified.");

	const float xPosIncrement = planeInfo.xEdgeLength / float(planeInfo.xEdgeVertexCount - 1);
	const float zPosIncrement = planeInfo.zEdgeLength / float(planeInfo.zEdgeVertexCount - 1);
	const float xUVIncrement = 1.f / float(planeInfo.xEdgeVertexCount - 1);
	const float zUVIncrement = 1.f / float(planeInfo.zEdgeVertexCount - 1);
	const float halfXEdgeLength = planeInfo.xEdgeLength / 2.f;
	const float halfZEdgeLength = planeInfo.zEdgeLength / 2.f;

	unsigned currentVertex = 0;
	unsigned currentIndex = 0;
	for (unsigned xVert = 0; xVert < planeInfo.xEdgeVertexCount; xVert++)
	{
		for (unsigned zVert = 0; zVert < planeInfo.zEdgeVertexCount; zVert++)
		{
			//vertices
			XMFLOAT3& position = reinterpret_cast<XMFLOAT3&>(reinterpret_cast<byte*>(out_positions)[positionByteStride * currentVertex]);
			position.x = xVert * xPosIncrement - halfXEdgeLength;
			position.y = 0.f;
			position.z = zVert * zPosIncrement - halfZEdgeLength;

			if (out_normals)
			{
				XMFLOAT3& normal = reinterpret_cast<XMFLOAT3&>(reinterpret_cast<byte*>(out_normals)[normalByteStride * currentVertex]);
				normal.x = 0.f;
				normal.y = 1.f;
				normal.z = 0.f;
			}
			
			if (out_uvs)
			{
				XMFLOAT2& uv = reinterpret_cast<XMFLOAT2&>(reinterpret_cast<byte*>(out_uvs)[uvByteStride * currentVertex]);
				uv.x = xVert * xUVIncrement;
				uv.y = zVert * zUVIncrement;
			}

			if (out_tangets)
			{
				XMFLOAT3& tangent = reinterpret_cast<XMFLOAT3&>(reinterpret_cast<byte*>(out_tangets)[tangentByteStride * currentVertex]);
				tangent.x = 1.f;
				tangent.y = 0.f;
				tangent.z = 0.f;
			}


			//indices
			if (xVert < (planeInfo.xEdgeVertexCount -1) && zVert < (planeInfo.zEdgeVertexCount -1))
			{
				// first triangle
				out_indices[currentIndex]	 = (planeInfo.zEdgeVertexCount*xVert) + zVert;
				out_indices[currentIndex +1] = (planeInfo.zEdgeVertexCount*xVert) + zVert +1;
				out_indices[currentIndex +2] = (planeInfo.zEdgeVertexCount*(xVert +1)) + zVert;

				// second triangle
				out_indices[currentIndex +3] = (planeInfo.zEdgeVertexCount*xVert) + zVert + 1;
				out_indices[currentIndex +4] = (planeInfo.zEdgeVertexCount*(xVert + 1)) + zVert +1;
				out_indices[currentIndex +5] = (planeInfo.zEdgeVertexCount*(xVert + 1)) + zVert;
				currentIndex += 6;
			}

			currentVertex++;
		}
	}
}



CylinderInfo xtest::mesh::GenerateCylinderInfo(unsigned sliceCount, unsigned stackCount, float height, float topRadius, float bottomRadius)
{
	XTEST_ASSERT(height > 0.f);
	XTEST_ASSERT(topRadius > 0.f);
	XTEST_ASSERT(bottomRadius > 0.f);
	XTEST_ASSERT(sliceCount > 1);
	XTEST_ASSERT(stackCount > 0);

	CylinderInfo info;
	info.sliceCount = sliceCount;
	info.stackCount = stackCount;
	info.height = height;
	info.topRadius = topRadius;
	info.bottomRadius = bottomRadius;

	// the top and the bottom bases are different vertices and have a vertex at the center

	const unsigned baseVertexCount = (sliceCount + 1);
	const unsigned sideVertexCount = sliceCount * (stackCount + 2);
	const unsigned baseIndexCount = sliceCount * 3;
	const unsigned sideIndexCount = (sliceCount * 6) * stackCount;

	info.vertexCount = baseVertexCount * 2 + sideVertexCount;
	info.indexCount = baseIndexCount * 2 + sideIndexCount;

	return info;
}


void xtest::mesh::GenerateCylinder(
	const CylinderInfo& cylinderInfo, 
	DirectX::XMFLOAT3* out_positions, 
	size_t positionByteStride, 
	DirectX::XMFLOAT3* out_normals, 
	size_t normalByteStride,
	DirectX::XMFLOAT2* out_uvs, 
	size_t uvStride, 
	DirectX::XMFLOAT3* out_tangets,
	size_t tangentByteStride, 
	uint32* out_indices)
{
	XTEST_UNUSED_VAR(uvStride);
	XTEST_UNUSED_VAR(tangentByteStride);

	XTEST_ASSERT(out_positions, L"Vertex positions destination must be specified.");
	XTEST_ASSERT(out_indices, L"Vertex indices destination must be specified.");

	const float halfHeight = cylinderInfo.height * 0.5f;
	const float sliceDeltaAngle = DirectX::XM_2PI / cylinderInfo.sliceCount;
	unsigned currentVertex = 0;
	unsigned currentIndex = 0;

	// bases
	{

		for (unsigned base = 0; base < 2; base++)
		{
			const bool isFacingUp = base == 0;
			const float baseHeight = isFacingUp ? halfHeight : -halfHeight;
			const float baseradius = isFacingUp ? cylinderInfo.topRadius : cylinderInfo.bottomRadius;
			

			// center vertex
			{
				XMFLOAT3& position = reinterpret_cast<XMFLOAT3&>(reinterpret_cast<byte*>(out_positions)[positionByteStride * currentVertex]);
				position = { 0.f, baseHeight, 0.f };
				
				if (out_normals)
				{
					XMFLOAT3& normal = reinterpret_cast<XMFLOAT3&>(reinterpret_cast<byte*>(out_normals)[normalByteStride * currentVertex]);
					normal = { 0.f, isFacingUp ? 1.f : -1.f , 0.f };
				}

				if (out_uvs)
				{
					// TODO
				}

				if (out_tangets)
				{
					// TODO
				}
			}
			currentVertex++;

			const unsigned centerVertexIndex = currentVertex -1;
			const unsigned previousVertexCount = currentVertex;

			for (unsigned sliceIndex = 0; sliceIndex < cylinderInfo.sliceCount; sliceIndex++)
			{
				// vertices
				XMFLOAT3& position = reinterpret_cast<XMFLOAT3&>(reinterpret_cast<byte*>(out_positions)[positionByteStride * currentVertex]);
				position.x = std::cosf(sliceDeltaAngle * sliceIndex) * baseradius;
				position.y = baseHeight;
				position.z = std::sinf(sliceDeltaAngle * sliceIndex) * baseradius;

				if (out_normals)
				{
					XMFLOAT3& normal = reinterpret_cast<XMFLOAT3&>(reinterpret_cast<byte*>(out_normals)[normalByteStride * currentVertex]);
					normal.x = 0.f;
					normal.y = isFacingUp ? 1.f : -1.f;
					normal.z = 0.f;
				}

				if (out_uvs)
				{
					// TODO
				}

				if (out_tangets)
				{
					// TODO
				}


				// indices
				if (sliceIndex < cylinderInfo.sliceCount -1)
				{

					out_indices[currentIndex] = centerVertexIndex;
					out_indices[currentIndex + 1] = previousVertexCount + (sliceIndex + 1);
					out_indices[currentIndex + 2] = previousVertexCount + sliceIndex;
					if (!isFacingUp)
					{
						std::swap(out_indices[currentIndex + 1], out_indices[currentIndex + 2]);
					}
					currentIndex += 3;
				}

				currentVertex++;
			}


			//close the circle
			out_indices[currentIndex] = centerVertexIndex;
			out_indices[currentIndex + 1] = previousVertexCount;
			out_indices[currentIndex + 2] = previousVertexCount + cylinderInfo.sliceCount -1;
			
			if (!isFacingUp)
			{
				std::swap(out_indices[currentIndex + 1], out_indices[currentIndex + 2]);
			}
			currentIndex += 3;

		}
	}



	// side part
	{
		const float deltaHeight = cylinderInfo.height / cylinderInfo.stackCount;
		const unsigned layerCount = cylinderInfo.stackCount + 1;
		const unsigned previousVertexCount = currentVertex;
		const unsigned layerVertexCount = cylinderInfo.sliceCount + 1;

		for (unsigned layerIndex = 0; layerIndex < layerCount; layerIndex++)
		{
			const float layerRadius = math::Lerp(cylinderInfo.topRadius, cylinderInfo.bottomRadius, float(cylinderInfo.stackCount - layerIndex) / float(cylinderInfo.stackCount));

			for (unsigned sliceIndex = 0; sliceIndex < layerVertexCount; sliceIndex++)
			{
				// vertices
				XMFLOAT3& position = reinterpret_cast<XMFLOAT3&>(reinterpret_cast<byte*>(out_positions)[positionByteStride * currentVertex]);
				position.x = std::cosf(sliceDeltaAngle * sliceIndex) * layerRadius;
				position.y = (layerIndex * deltaHeight) - halfHeight;
				position.z = std::sinf(sliceDeltaAngle * sliceIndex) * layerRadius;

				if (out_normals)
				{
					// TODO
				}

				if (out_uvs)
				{
					// TODO
				}

				if (out_tangets)
				{
					// TODO
				}

				// indices
				if (sliceIndex < (layerVertexCount -1) && layerIndex < (layerCount -1))
				{
					out_indices[currentIndex] = previousVertexCount + (sliceIndex + (layerVertexCount * layerIndex));
					out_indices[currentIndex + 1] = previousVertexCount + (sliceIndex + (layerVertexCount * (layerIndex + 1)));
					out_indices[currentIndex + 2] = previousVertexCount + ((sliceIndex + 1) + (layerVertexCount * layerIndex));
					out_indices[currentIndex + 3] = previousVertexCount + ((sliceIndex + 1) + (layerVertexCount * layerIndex));
					out_indices[currentIndex + 4] = previousVertexCount + (sliceIndex + (layerVertexCount * (layerIndex + 1)));
					out_indices[currentIndex + 5] = previousVertexCount + ((sliceIndex + 1) + (layerVertexCount * (layerIndex + 1)));
					currentIndex += 6;
				}
				
				currentVertex++;
			}
		}
	}


}
