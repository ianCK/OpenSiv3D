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

# include <Siv3D/Physics2D/P2Triangle.hpp>
# include "P2Common.hpp"

namespace s3d
{
	P2Triangle::P2Triangle(b2Body& body, const Triangle& triangle, const P2Material& material, const P2Filter& filter)
		: m_pShape{ std::make_unique<b2PolygonShape>() }
	{
		const b2Vec2 points[3] = { detail::ToB2Vec2(triangle.p0), detail::ToB2Vec2(triangle.p1), detail::ToB2Vec2(triangle.p2) };

		m_pShape->Set(points, 3);

		const b2FixtureDef fixtureDef = detail::MakeFixtureDef(m_pShape.get(), material, filter);

		m_fixtures.push_back(body.CreateFixture(&fixtureDef));
	}

	P2ShapeType P2Triangle::getShapeType() const noexcept
	{
		return P2ShapeType::Triangle;
	}

	void P2Triangle::draw(const ColorF& color) const
	{
		getTriangle().draw(color);
	}

	void P2Triangle::drawFrame(const double thickness, const ColorF& color) const
	{
		getTriangle().drawFrame(detail::AdjustThickness(thickness), color);
	}

	void P2Triangle::drawWireframe(const double thickness, const ColorF& color) const
	{
		drawFrame(thickness, color);
	}

	Triangle P2Triangle::getTriangle() const
	{
		const b2Transform& transform = m_fixtures.front()->GetBody()->GetTransform();
		return{
			detail::CalcVec2(m_pShape->m_vertices[0], transform),
			detail::CalcVec2(m_pShape->m_vertices[1], transform),
			detail::CalcVec2(m_pShape->m_vertices[2], transform) };
	}
}
