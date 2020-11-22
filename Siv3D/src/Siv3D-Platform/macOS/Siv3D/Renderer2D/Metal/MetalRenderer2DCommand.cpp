﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2020 Ryo Suzuki
//	Copyright (c) 2016-2020 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "MetalRenderer2DCommand.hpp"

namespace s3d
{
	MetalRenderer2DCommandManager::MetalRenderer2DCommandManager()
	{
		reset();
	}

	void MetalRenderer2DCommandManager::reset()
	{
		// clear commands
		{
			m_commands.clear();
			m_changes.clear();
		}

		// clear buffers
		{
			m_draws.clear();
			m_nullDraws.clear();
		}

		// clear reserves
		{
			m_reservedVSs.clear();
			m_reservedPSs.clear();
		}

		// Begin a new frame
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::SetBuffers, 0);
			m_commands.emplace_back(MetalRenderer2DCommandType::UpdateBuffers, 0);

			m_VSs = { VertexShader::IDType::InvalidValue() };
			m_commands.emplace_back(MetalRenderer2DCommandType::SetVS, 0);
			m_currentVS = VertexShader::IDType::InvalidValue();

			m_PSs = { PixelShader::IDType::InvalidValue() };
			m_commands.emplace_back(MetalRenderer2DCommandType::SetPS, 0);
			m_currentPS = PixelShader::IDType::InvalidValue();
		}
	}

	void MetalRenderer2DCommandManager::flush()
	{
		if (m_currentDraw.indexCount)
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::Draw, static_cast<uint32>(m_draws.size()));
			m_draws.push_back(m_currentDraw);
			m_currentDraw.indexCount = 0;
		}

		if (m_changes.has(MetalRenderer2DCommandType::SetBuffers))
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::SetBuffers, 0);
		}

		if (m_changes.has(MetalRenderer2DCommandType::SetVS))
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::SetVS, static_cast<uint32>(m_VSs.size()));
			m_VSs.push_back(m_currentVS);
		}

		if (m_changes.has(MetalRenderer2DCommandType::SetPS))
		{
			m_commands.emplace_back(MetalRenderer2DCommandType::SetPS, static_cast<uint32>(m_PSs.size()));
			m_PSs.push_back(m_currentPS);
		}

		m_changes.clear();
	}

	const Array<MetalRenderer2DCommand>& MetalRenderer2DCommandManager::getCommands() const noexcept
	{
		return m_commands;
	}

	void MetalRenderer2DCommandManager::pushUpdateBuffers(const uint32 batchIndex)
	{
		flush();

		m_commands.emplace_back(MetalRenderer2DCommandType::UpdateBuffers, batchIndex);
	}

	void MetalRenderer2DCommandManager::pushDraw(const Vertex2D::IndexType indexCount)
	{
		if (m_changes.hasStateChange())
		{
			flush();
		}

		m_currentDraw.indexCount += indexCount;
	}

	const MetalDrawCommand& MetalRenderer2DCommandManager::getDraw(const uint32 index) const noexcept
	{
		return m_draws[index];
	}

	void MetalRenderer2DCommandManager::pushNullVertices(const uint32 count)
	{
		if (m_changes.hasStateChange())
		{
			flush();
		}

		m_commands.emplace_back(MetalRenderer2DCommandType::DrawNull, static_cast<uint32>(m_nullDraws.size()));
		m_nullDraws.push_back(count);
		m_changes.set(MetalRenderer2DCommandType::DrawNull);
	}

	uint32 MetalRenderer2DCommandManager::getNullDraw(const uint32 index) const noexcept
	{
		return m_nullDraws[index];
	}

	void MetalRenderer2DCommandManager::pushStandardVS(const VertexShader::IDType& id)
	{
		constexpr auto command = MetalRenderer2DCommandType::SetVS;
		auto& current = m_currentVS;
		auto& buffer = m_VSs;

		if (not m_changes.has(command))
		{
			if (id != current)
			{
				current = id;
				m_changes.set(command);
			}
		}
		else
		{
			if (id == buffer.back())
			{
				current = id;
				m_changes.clear(command);
			}
			else
			{
				current = id;
			}
		}
	}

	void MetalRenderer2DCommandManager::pushCustomVS(const VertexShader& vs)
	{
		const auto id = vs.id();
		constexpr auto command = MetalRenderer2DCommandType::SetVS;
		auto& current = m_currentVS;
		auto& buffer = m_VSs;

		if (not m_changes.has(command))
		{
			if (id != current)
			{
				current = id;
				m_changes.set(command);
				m_reservedVSs.try_emplace(id, vs);
			}
		}
		else
		{
			if (id == buffer.back())
			{
				current = id;
				m_changes.clear(command);
			}
			else
			{
				current = id;
				m_reservedVSs.try_emplace(id, vs);
			}
		}
	}

	const VertexShader::IDType& MetalRenderer2DCommandManager::getVS(const uint32 index) const
	{
		return m_VSs[index];
	}

	void MetalRenderer2DCommandManager::pushStandardPS(const PixelShader::IDType& id)
	{
		constexpr auto command = MetalRenderer2DCommandType::SetPS;
		auto& current = m_currentPS;
		auto& buffer = m_PSs;

		if (not m_changes.has(command))
		{
			if (id != current)
			{
				current = id;
				m_changes.set(command);
			}
		}
		else
		{
			if (id == buffer.back())
			{
				current = id;
				m_changes.clear(command);
			}
			else
			{
				current = id;
			}
		}
	}

	void MetalRenderer2DCommandManager::pushCustomPS(const PixelShader& ps)
	{
		const auto id = ps.id();
		constexpr auto command = MetalRenderer2DCommandType::SetPS;
		auto& current = m_currentPS;
		auto& buffer = m_PSs;

		if (not m_changes.has(command))
		{
			if (id != current)
			{
				current = id;
				m_changes.set(command);
				m_reservedPSs.try_emplace(id, ps);
			}
		}
		else
		{
			if (id == buffer.back())
			{
				current = id;
				m_changes.clear(command);
			}
			else
			{
				current = id;
				m_reservedPSs.try_emplace(id, ps);
			}
		}
	}

	const PixelShader::IDType& MetalRenderer2DCommandManager::getPS(const uint32 index) const
	{
		return m_PSs[index];
	}
}
