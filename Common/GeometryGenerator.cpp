//***************************************************************************************
// GeometryGenerator.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "GeometryGenerator.h"
#include <algorithm>
#include <windows.h>
#include <fstream>
#include <bitset>
#include "ImprovedNoise.h"




using namespace DirectX;

void GeometryGenerator::ChangeDirection(Vertex vert, DirectX::XMVECTOR& vertexnormal)
{
	if (vert.edgenormal[0] == true)
	{
		vertexnormal.m128_f32[2] = signbit(vertexnormal.m128_f32[2]) ? vertexnormal.m128_f32[2] * 1.f : vertexnormal.m128_f32[2] * -1.f;
	}
	else
	{
		vertexnormal.m128_f32[2] = signbit(vertexnormal.m128_f32[2]) ? vertexnormal.m128_f32[2] * -1.f : vertexnormal.m128_f32[2] * 1.f;
	}
	if (vert.edgenormal[1] == true)
	{
		vertexnormal.m128_f32[0] = signbit(vertexnormal.m128_f32[0]) ? vertexnormal.m128_f32[0] * 1.f : vertexnormal.m128_f32[0] * -1.f;
	}
	else
	{
		vertexnormal.m128_f32[0] = signbit(vertexnormal.m128_f32[0]) ? vertexnormal.m128_f32[0] * -1.f : vertexnormal.m128_f32[0] * 1.f;
	}
	if (vert.edgenormal[2] == true)
	{
		vertexnormal.m128_f32[1] = signbit(vertexnormal.m128_f32[1]) ? vertexnormal.m128_f32[1] * 1.f : vertexnormal.m128_f32[1] * -1.f;
	}
	else
	{
		vertexnormal.m128_f32[1] = signbit(vertexnormal.m128_f32[1]) ? vertexnormal.m128_f32[1] * -1.f : vertexnormal.m128_f32[1] * 1.f;
	}

}
GeometryGenerator::MeshData GeometryGenerator::CreateCubeGrid(float width, float height, float depth)
{
	ImprovedNoise::ImprovedNoise();
	MeshData meshData;
	uint32_t nRows = (depth * 2) + 1, nHeight = (height * 2) + 1, nCols = (width * 2) + 1;
	uint32_t vertexCount = nCols * nRows * nHeight;
	uint32_t faceCount = (nCols - 1) * (nRows - 1) * (nHeight - 1) * 6;
	float halfWidth = 0.5f * width;
	float halfheight = 0.5f * height;
	float halfDepth = 0.5f * depth;
	float dx = width / (nCols - 1);
	float dy = height / (nHeight - 1);
	float dz = depth / (nRows - 1);

	int scale = 15;
	meshData.Vertices.resize(vertexCount);
	meshData.Density.resize(vertexCount);
	;
	float density;	std::ofstream stream;
	for (int height = 0; height < nHeight; ++height)
	{
		//uncomment to generate ppm image of da terrain
		/*stream.open(".\\Noise\\noise" + std::to_string(height) + ".ppm", std::ios::out | std::ios::binary);
		stream << "P6" << std::endl << nCols << " " << nRows << std::endl << "255" << std::endl;*/
		float y = -halfheight + (height * dy);
		for (int column = 0; column < nCols; ++column)
		{
			float x = -halfWidth + (column * dx);
			for (int row = 0; row < nRows; ++row)
			{

				float z = -halfDepth + (row * dz);
				int currentIndex = ((height * nRows * nCols)) + ((column * nRows)) + row;

				meshData.Vertices[currentIndex].pos = DirectX::XMVectorSet(x, y, z, 0);

				density = .25 * ImprovedNoise::Noise(4.03 * x / scale, 4.03 * z / scale, 4.03 * y / scale);
				density += .5 * ImprovedNoise::Noise(1.96 * x / scale, 1.96 * z / scale, 1.96 * y / scale);
				density += ImprovedNoise::Noise(1.01 * x / scale, 1.01 * z / scale, 1.01 * y / scale);
				density = ((y / 4)) - floor(density);
				meshData.Density[currentIndex] = density;

				//meshData.Vertices[currentIndex].texCoord3D = XMFLOAT3(column/**du*/, nHeight - height, row /**dv*/);
				//uncomment to generate ppm image of da terrain
			   // unsigned char n = static_cast<unsigned char>(floor(255 * density));
			   // stream << n << n << n;


			}
		}
		stream.close();
	}


	Vertex vert = {};
	std::bitset<8> bin;

	for (int height = 0; height < nHeight - 1; ++height)
	{
		for (int column = 0; column < nCols - 1; ++column)
		{
			for (int row = 0; row < nRows - 1; ++row)
			{
				bin = {}; vert = {};
				int currentIndex = (height * nRows * nCols) + (column * nRows) + row;
				int index1 = (height * nRows * nCols) + (column * nRows) + (row + 1);
				int index2 = (height * nRows * nCols) + (column * nRows) + row + nRows;
				int index3 = (height * nRows * nCols) + (column * nRows) + row + 1 + nRows;
				int index4 = ((height + 1) * nRows * nCols) + (column * nRows) + row;
				int index5 = ((height + 1) * nRows * nCols) + (column * nRows) + row + 1;
				int index6 = ((height + 1) * nRows * nCols) + ((column + 1) * nRows) + row;
				int index7 = ((height + 1) * nRows * nCols) + ((column + 1) * nRows) + row + 1;

				bin[0] = signbit(meshData.Density[currentIndex]);
				bin[1] = signbit(meshData.Density[index1]);
				bin[2] = signbit(meshData.Density[index2]);
				bin[3] = signbit(meshData.Density[index3]);
				bin[4] = signbit(meshData.Density[index4]);
				bin[5] = signbit(meshData.Density[index5]);
				bin[6] = signbit(meshData.Density[index6]);
				bin[7] = signbit(meshData.Density[index7]);

				if (bin.none() || bin.all())
				{
				}
				else
				{
					{
						DirectX::XMVECTOR v0 = meshData.Vertices[currentIndex].pos;
						DirectX::XMVECTOR v1 = meshData.Vertices[index1].pos;
						DirectX::XMVECTOR v2 = meshData.Vertices[index2].pos;
						DirectX::XMVECTOR v3 = meshData.Vertices[index3].pos;
						DirectX::XMVECTOR v4 = meshData.Vertices[index4].pos;
						DirectX::XMVECTOR v5 = meshData.Vertices[index5].pos;
						DirectX::XMVECTOR v6 = meshData.Vertices[index6].pos;
						DirectX::XMVECTOR v7 = meshData.Vertices[index7].pos;
						auto center = DirectX::XMVectorAdd(v0, v1);
						center = DirectX::XMVectorAdd(center, v2);
						center = DirectX::XMVectorAdd(center, v3);
						center = DirectX::XMVectorAdd(center, v4);
						center = DirectX::XMVectorAdd(center, v5);
						center = DirectX::XMVectorAdd(center, v6);
						center = DirectX::XMVectorAdd(center, v7);
						center = DirectX::XMVectorScale(center, .125f);

						vert.pos = center;

						//vert.texCoord = meshData.Vertices[currentIndex].texCoord;
						if (bin[6] == bin[7])
						{
						}
						else
						{
							meshData.Vertices[index6].pos = DirectX::XMVectorAdd(center, DirectX::XMVectorSet(dx, dy, 0, 0));
							// meshData.Vertices[index6].texCoord= meshData.Vertices[index6].texCoord;
							meshData.Vertices[index6].isVertexCreated = true;

							meshData.Vertices[index2].pos = DirectX::XMVectorAdd(center, DirectX::XMVectorSet(dx, 0, 0, 0));
							// meshData.Vertices[index2].texCoord = meshData.Vertices[index2].texCoord;
							meshData.Vertices[index2].isVertexCreated = true;

							meshData.Vertices[index4].pos = DirectX::XMVectorAdd(center, DirectX::XMVectorSet(0, dy, 0, 0));
							// meshData.Vertices[index4].texCoord = meshData.Vertices[index4].texCoord;
							meshData.Vertices[index4].isVertexCreated = true;
							vert.signedEdges[10] = true;
							vert.isVertexCreated = true;
							if (bin[6] == true)
							{
								vert.edgenormal[0] = true;//z= ensure z is negative
							}
							meshData.Vertices[currentIndex] = vert;

						}
						if (bin[5] == bin[7])
						{
						}
						else
						{
							meshData.Vertices[index1].pos = DirectX::XMVectorAdd(center, DirectX::XMVectorSet(0, 0, dz, 0));
							// meshData.Vertices[index1].texCoord = meshData.Vertices[index1].texCoord;
							meshData.Vertices[index1].isVertexCreated = true;

							meshData.Vertices[index5].pos = DirectX::XMVectorAdd(center, DirectX::XMVectorSet(0, dy, dz, 0));
							// meshData.Vertices[index5].texCoord = meshData.Vertices[index5].texCoord;
							meshData.Vertices[index5].isVertexCreated = true;

							meshData.Vertices[index4].pos = DirectX::XMVectorAdd(center, DirectX::XMVectorSet(0, dy, 0, 0));
							// meshData.Vertices[index4].texCoord = meshData.Vertices[index4].texCoord;
							meshData.Vertices[index4].isVertexCreated = true;
							vert.signedEdges[1] = true;
							vert.isVertexCreated = true;
							if (bin[5] == true)
							{
								vert.edgenormal[1] = true;//z= ensure x is negative
							}
							meshData.Vertices[currentIndex] = vert;
						}
						if (bin[3] == bin[7])
						{
						}
						else
						{
							meshData.Vertices[index1].pos = DirectX::XMVectorAdd(center, DirectX::XMVectorSet(0, 0, dz, 0));
							// meshData.Vertices[index1].texCoord = meshData.Vertices[index1].texCoord;
							meshData.Vertices[index1].isVertexCreated = true;

							meshData.Vertices[index2].pos = DirectX::XMVectorAdd(center, DirectX::XMVectorSet(dx, 0, 0, 0));
							// meshData.Vertices[index2].texCoord = meshData.Vertices[index2].texCoord;
							meshData.Vertices[index2].isVertexCreated = true;

							meshData.Vertices[index3].pos = DirectX::XMVectorAdd(center, DirectX::XMVectorSet(dx, 0, dz, 0));
							// meshData.Vertices[index3].texCoord = meshData.Vertices[index3].texCoord;
							meshData.Vertices[index3].isVertexCreated = true;
							vert.signedEdges[2] = true;
							vert.isVertexCreated = true;
							if (bin[3] == true)
							{
								vert.edgenormal[2] = true;//z= ensure y is negative
							}
							meshData.Vertices[currentIndex] = vert;
						}
					}
				}
			}
		}
	}

	dx = (float)3 / nRows;
	dy = (float)3 / nHeight;
	dz = (float)3 / nCols;

	for (int height = 0; height < nHeight - 1; ++height)
	{
		for (int column = 0; column < nCols - 1; ++column)
		{
			for (int row = 0; row < nRows - 1; ++row)
			{
				int index = (height * nRows * nCols) + (column * nRows) + row;
				int index1 = (height * nRows * nCols) + (column * nRows) + (row + 1);
				int index2 = (height * nRows * nCols) + (column * nRows) + row + nRows;
				int index3 = (height * nRows * nCols) + (column * nRows) + row + 1 + nRows;
				int index4 = ((height + 1) * nRows * nCols) + (column * nRows) + row;
				int index5 = ((height + 1) * nRows * nCols) + (column * nRows) + row + 1;
				int index6 = ((height + 1) * nRows * nCols) + ((column + 1) * nRows) + row;
				int index7 = ((height + 1) * nRows * nCols) + ((column + 1) * nRows) + row + 1;
				vert = meshData.Vertices[index];
				XMVECTOR v2 = meshData.Vertices[index2].pos;
				XMVECTOR v = meshData.Vertices[index].pos;
				XMVECTOR v1 = meshData.Vertices[index1].pos;
				XMVECTOR v3 = meshData.Vertices[index3].pos;
				XMVECTOR v4 = meshData.Vertices[index4].pos;
				XMVECTOR v5 = meshData.Vertices[index5].pos;
				XMVECTOR v6 = meshData.Vertices[index6].pos;
				XMVECTOR v7 = meshData.Vertices[index7].pos;

				if (vert.isVertexCreated)
				{

					int count = 0;

					if (vert.signedEdges[1])
					{
						XMVECTOR N = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v, v1), DirectX::XMVectorSubtract(v, v4));
						XMVECTOR a = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(v, v1), DirectX::XMVectorSubtract(v, v4));
						ChangeDirection(vert, N);

						XMVECTOR N1 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v1, v), DirectX::XMVectorSubtract(v1, v5));
						XMVECTOR a1 = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(v1, v), DirectX::XMVectorSubtract(v1, v5));
						ChangeDirection(vert, N1);
						XMVECTOR N5 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v5, v4), DirectX::XMVectorSubtract(v5, v1));
						XMVECTOR a5 = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(v5, v4), DirectX::XMVectorSubtract(v5, v1));
						ChangeDirection(vert, N5);
						XMVECTOR N4 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v4, v), DirectX::XMVectorSubtract(v4, v5));
						XMVECTOR a4 = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(v4, v), DirectX::XMVectorSubtract(v4, v5));
						ChangeDirection(vert, N4);
						meshData.Vertices[index].normal += N;
						meshData.Vertices[index1].normal += N;
						meshData.Vertices[index4].normal += N;
						meshData.Vertices[index5].normal += N5;
						meshData.Vertices[index4].normal += N5;
						meshData.Vertices[index1].normal += N5;
						if (signbit(N.m128_f32[0]) == false)
						{
							meshData.Indices32.push_back(index1);
							meshData.Indices32.push_back(index5);
							meshData.Indices32.push_back(index4);
							meshData.Indices32.push_back(index1);
							meshData.Indices32.push_back(index4);
							meshData.Indices32.push_back(index);

						}
						else
						{
							meshData.Indices32.push_back(index);
							meshData.Indices32.push_back(index4);
							meshData.Indices32.push_back(index5);
							meshData.Indices32.push_back(index);
							meshData.Indices32.push_back(index5);
							meshData.Indices32.push_back(index1);

						}
					}
					if (vert.signedEdges[10])
					{
						XMVECTOR N = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v, v4), DirectX::XMVectorSubtract(v, v2));
						XMVECTOR a = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(v, v4), DirectX::XMVectorSubtract(v, v2));
						ChangeDirection(vert, N);
						XMVECTOR N2 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v2, v), DirectX::XMVectorSubtract(v2, v6));
						XMVECTOR a2 = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(v2, v), DirectX::XMVectorSubtract(v2, v6));
						ChangeDirection(vert, N2);
						XMVECTOR N6 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v6, v4), DirectX::XMVectorSubtract(v6, v2));
						XMVECTOR a6 = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(v6, v4), DirectX::XMVectorSubtract(v6, v2));
						ChangeDirection(vert, N6);
						XMVECTOR N4 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v4, v), DirectX::XMVectorSubtract(v4, v6));
						XMVECTOR a4 = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(v4, v), DirectX::XMVectorSubtract(v4, v6));
						ChangeDirection(vert, N4);
						meshData.Vertices[index].normal += N;
						meshData.Vertices[index4].normal += N;
						meshData.Vertices[index2].normal += N;
						meshData.Vertices[index6].normal += N6;
						meshData.Vertices[index4].normal += N6;
						meshData.Vertices[index2].normal += N6;
						if (signbit(N.m128_f32[2]) == false)
						{
							meshData.Indices32.push_back(index2);
							meshData.Indices32.push_back(index);
							meshData.Indices32.push_back(index4);
							meshData.Indices32.push_back(index2);
							meshData.Indices32.push_back(index4);
							meshData.Indices32.push_back(index6);

						}
						else
						{

							meshData.Indices32.push_back(index4);
							meshData.Indices32.push_back(index);
							meshData.Indices32.push_back(index2);
							meshData.Indices32.push_back(index4);
							meshData.Indices32.push_back(index2);
							meshData.Indices32.push_back(index6);
						}
					}
					if (vert.signedEdges[2])
					{
						XMVECTOR N = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v, v1), DirectX::XMVectorSubtract(v, v2));
						XMVECTOR A = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(v, v1), DirectX::XMVectorSubtract(v, v2));
						ChangeDirection(vert, N);
						XMVECTOR N1 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v1, v), DirectX::XMVectorSubtract(v1, v3));
						XMVECTOR A1 = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(v1, v), DirectX::XMVectorSubtract(v1, v3));
						ChangeDirection(vert, N1);
						XMVECTOR N2 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v2, v), DirectX::XMVectorSubtract(v2, v3));
						XMVECTOR A2 = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(v2, v), DirectX::XMVectorSubtract(v2, v3));
						ChangeDirection(vert, N2);
						XMVECTOR N3 = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v3, v1), DirectX::XMVectorSubtract(v3, v2));
						XMVECTOR A3 = DirectX::XMVector3Dot(DirectX::XMVectorSubtract(v3, v1), DirectX::XMVectorSubtract(v3, v2));
						ChangeDirection(vert, N3);

						meshData.Vertices[index1].normal += N1;
						meshData.Vertices[index].normal += N1;
						meshData.Vertices[index3].normal += N1;
						meshData.Vertices[index2].normal += N2;
						meshData.Vertices[index].normal += N2;
						meshData.Vertices[index3].normal += N2;

						if (signbit(N.m128_f32[1]) == false)
						{
							meshData.Indices32.push_back(index);
							meshData.Indices32.push_back(index2);
							meshData.Indices32.push_back(index3);
							meshData.Indices32.push_back(index);
							meshData.Indices32.push_back(index3);
							meshData.Indices32.push_back(index1);

						}
						else
						{

							meshData.Indices32.push_back(index);
							meshData.Indices32.push_back(index1);
							meshData.Indices32.push_back(index3);
							meshData.Indices32.push_back(index);
							meshData.Indices32.push_back(index3);
							meshData.Indices32.push_back(index2);

						}
					}
				}
			}
		}
	}
	return meshData;
}
