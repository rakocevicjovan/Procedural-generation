#pragma once
#include "Mesh.h"
#include "CollisionEngine.h"
#include "Geometry.h"
#include "Terrain.h"

Mesh::Mesh()
{
	_vertexBuffer = 0;
	_indexBuffer = 0;
}



Mesh::Mesh(std::vector<Vert3D> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, ID3D11Device* device, unsigned int ind)
	: vertices(vertices), indices(indices), textures(textures)
{
	_vertexBuffer = 0;
	_indexBuffer = 0;

	indexIntoModelMeshArray = ind;
	setupMesh(device);	// Now that we have all the required data, set the vertex buffers and its attribute pointers.
}



Mesh::Mesh(const Procedural::Terrain& terrain, ID3D11Device* device)
{
	terrain.populateMesh(vertices, indices, textures);
	setupMesh(device);
}



Mesh::Mesh(const SVec2& pos, const SVec2& size, ID3D11Device* device)
{
	float originX = (pos.x - 0.5f) * 2.f;	//[0, 1] -> [-1, 1]
	float originY = (pos.y - 0.5f) * 2.f;
	float width = size.x * 2.f;
	float height = size.y * 2.f;

	Vert3D topLeft;
	topLeft.pos = SVec3(originX, originY + height, 0.0f);
	topLeft.texCoords = SVec2(0.f, 1.f);

	Vert3D topRight;
	topRight.pos = SVec3(originX + width, originY + height, 0.0f);
	topRight.texCoords = SVec2(1.f, 1.f);

	Vert3D bottomLeft;
	bottomLeft.pos = SVec3(originX, originY, 0.0f);
	bottomLeft.texCoords = SVec2(0.f, 0.f);

	Vert3D bottomRight;
	bottomRight.pos = SVec3(originX + width, originY, 0.0f);
	bottomRight.texCoords = SVec2(1.f, 0.f);

	vertices.push_back(topLeft);
	vertices.push_back(topRight);
	vertices.push_back(bottomLeft);
	vertices.push_back(bottomRight);

	indices = std::vector<unsigned int>{ 0u, 1u, 2u, 1u, 3u, 2u };

	setupMesh(device);
}



Mesh::Mesh(const Procedural::Geometry& g, ID3D11Device* device, bool setUp, bool hasTangents)
{
	vertices.reserve(g.positions.size());
	Vert3D v;

	for (int i = 0; i < g.positions.size(); ++i)
	{
		v.pos = g.positions[i];
		v.texCoords = g.texCoords[i];
		v.normal = g.normals[i];
		if(hasTangents)
			v.tangent = g.tangents[i];
		vertices.push_back(v);
	}

	indices = g.indices;

	if(setUp)
		setupMesh(device);
}



Mesh::Mesh(Hull* hull, ID3D11Device* device)
{
	AABB* aabb = reinterpret_cast<AABB*>(hull);

	SVec3 sizes = (aabb->maxPoint - aabb->minPoint);
	SVec3 offset = aabb->minPoint + sizes * 0.5f;

	Procedural::Geometry g;
	g.GenBox(sizes);

	vertices.reserve(g.positions.size());
	Vert3D v;

	for (int i = 0; i < g.positions.size(); ++i)
	{
		v.pos = g.positions[i] + offset;
		v.normal = g.normals[i];
		vertices.push_back(v);
	}

	indices = g.indices;

	setupMesh(device);
}



bool Mesh::setupMesh(ID3D11Device* device) //, D3D11_BUFFER_DESC vertexBufferDesc, D3D11_BUFFER_DESC indexBufferDesc)
{
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	vertexBufferDesc.ByteWidth = sizeof(Vert3D) * vertices.size();
	vertexData.pSysMem = vertices.data();

	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer)))
		return false;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer)))
		return false;

	return true;
}