//***************************************************************************************
// GeometryGenerator.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Defines a static class for procedurally generating the geometry of 
// common mathematical objects.
//
// All triangles are generated "outward" facing.  If you want "inward" 
// facing triangles (for example, if you want to place the camera inside
// a sphere to simulate a sky), you will need to:
//   1. Change the Direct3D cull mode or manually reverse the winding order.
//   2. Invert the normal.
//   3. Update the texture coordinates and tangent vectors.
//***************************************************************************************

#pragma once

#include <cstdint>
#include <DirectXMath.h>
#include <vector>
#include <bitset>
#include <array>


class GeometryGenerator
{
public:

	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;

	struct Vertex
	{
		Vertex() = default;
		Vertex(
			const DirectX::XMVECTOR& pos,
			const DirectX::XMVECTOR& norm,
			const DirectX::XMFLOAT3& tang,
			const DirectX::XMFLOAT2& uv) :
			pos(pos),
			normal(norm),
			tangentU(tang),
			texCoord(uv) {}
		Vertex(
			float posx, float posy, float posz,
			float normx, float normy, float normz,
			float tangx, float tangy, float tangz,
			float u, float v) :

			pos(DirectX::XMVectorSet(posx, posy, posz, 0)),
			normal(DirectX::XMVectorSet(normx, normy, normz, 0)),
			tangentU(tangx, tangy, tangz),
			texCoord(u, v) {}

		DirectX::XMVECTOR pos;
		std::array<int, 3> voxelIndex; //height column  row  
		DirectX::XMVECTOR Gradientnormal;
		DirectX::XMVECTOR normal;
		DirectX::XMFLOAT3 tangentU;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT3 texCoord3D;
		//front back top bottom right left
		std::bitset<12> signedEdges;
		std::bitset<3> edgenormal;
		bool isVertexCreated;
		bool winding;



	};

	struct MeshData
	{
		std::vector<float> Density;
		std::vector<Vertex> Vertices;

		std::vector<uint32> Indices32;

		std::vector<uint16>& GetIndices16()
		{
			if (mIndices16.empty())
			{
				mIndices16.resize(Indices32.size());
				for (size_t i = 0; i < Indices32.size(); ++i)
					mIndices16[i] = static_cast<uint16>(Indices32[i]);
			}

			return mIndices16;
		}

	private:
		std::vector<uint16> mIndices16;
	};

	///<summary>
	/// Creates a box centered at the origin with the given dimensions, where each
	/// face has m rows and n columns of vertices.
	///</summary>
	MeshData CreateBox(float width, float height, float depth, uint32 numSubdivisions);

	///<summary>
	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	///</summary>
	MeshData CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);

	///<summary>
	/// Creates a geosphere centered at the origin with the given radius.  The
	/// depth controls the level of tessellation.
	///</summary>
	MeshData CreateGeosphere(float radius, uint32 numSubdivisions);

	///<summary>
	/// Creates a cylinder parallel to the y-axis, and centered about the origin.  
	/// The bottom and top radius can vary to form various cone shapes rather than true
	// cylinders.  The slices and stacks parameters control the degree of tessellation.
	///</summary>
	MeshData CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);

	///<summary>
	/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
	/// at the origin with the specified width and depth.
	///</summary>
	MeshData CreateGrid(float width, float depth, uint32 m, uint32 n);

	MeshData CreateGrid2(float width = 100, float depth = 100, size_t divisions = 100, float scale = 1.f);

	///<summary>
	/// Creates a quad aligned with the screen.  This is useful for postprocessing and screen effects.
	///</summary>
	MeshData CreateQuad(float x, float y, float w, float h, float depth);

	
	void ChangeDirection(Vertex vert, DirectX::XMVECTOR& vertexnormal);

	GeometryGenerator::MeshData CreateCubeGrid(float width, float height, float depth);

private:
	void Subdivide(MeshData& meshData);
	Vertex MidPoint(const Vertex& v0, const Vertex& v1);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
};

