﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2021 Ryo Suzuki
//	Copyright (c) 2016-2021 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "GL4Mesh.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	GL4Mesh::GL4Mesh(const MeshData& meshData)
		: GL4Mesh{ meshData.vertices, meshData.indices } {}

	GL4Mesh::GL4Mesh(const Array<Vertex3D>& vertices, const Array<TriangleIndex32>& indices)
		: m_vertexCount{ static_cast<uint32>(vertices.size()) }
		, m_indexCount{ static_cast<uint32>(indices.size() * 3) }
		, m_vertexStride{ sizeof(Vertex3D) }
	{
		::glGenVertexArrays(1, &m_vao);
		::glGenBuffers(1, &m_vertexBuffer);
		::glGenBuffers(1, &m_indexBuffer);

		::glBindVertexArray(m_vao);
		{
			{
				::glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
				::glBufferData(GL_ARRAY_BUFFER, (m_vertexStride * m_vertexCount), vertices.data(), GL_DYNAMIC_DRAW);
			}

			{
				::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, (const GLubyte*)0);	// Vertex3D::pos
				::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 32, (const GLubyte*)12);	// Vertex3D::normal
				::glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 32, (const GLubyte*)24);	// Vertex3D::tex

				::glEnableVertexAttribArray(0);
				::glEnableVertexAttribArray(1);
				::glEnableVertexAttribArray(2);
			}

			{
				::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
				::glBufferData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(TriangleIndex32::value_type) * m_indexCount), indices.data(), GL_DYNAMIC_DRAW);
			}
		}
		::glBindVertexArray(0);

		m_initialized = true;
	}

	GL4Mesh::~GL4Mesh()
	{
		if (m_indexBuffer)
		{
			::glDeleteBuffers(1, &m_indexBuffer);
			m_indexBuffer = 0;
		}

		if (m_vertexBuffer)
		{
			::glDeleteBuffers(1, &m_vertexBuffer);
			m_vertexBuffer = 0;
		}

		if (m_vao)
		{
			::glDeleteVertexArrays(1, &m_vao);
			m_vao = 0;
		}
	}

	bool GL4Mesh::isInitialized() const noexcept
	{
		return m_initialized;
	}

	uint32 GL4Mesh::getVertexCount() const noexcept
	{
		return m_vertexCount;
	}

	uint32 GL4Mesh::getIndexCount() const noexcept
	{
		return m_indexCount;
	}

	void GL4Mesh::bindToContext()
	{
		::glBindVertexArray(m_vao);
		::glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	}
}