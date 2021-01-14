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

# pragma once

namespace s3d
{
	inline Polygon::operator bool() const noexcept
	{
		return (not isEmpty());
	}

	inline void Polygon::swap(Polygon& polygon) noexcept
	{
		std::swap(pImpl, polygon.pImpl);
	}

	template <class Shape2DType>
	inline bool Polygon::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersect(*this, other);
	}

	template <class Shape2DType>
	inline Optional<Array<Vec2>> Polygon::intersectsAt(const Shape2DType& other) const
	{
		return Geometry2D::IntersectAt(*this, other);
	}

	template <class Shape2DType>
	inline bool Polygon::contains(const Shape2DType& other) const
	{
		return Geometry2D::Contains(*this, other);
	}

	inline PolygonFailureType Polygon::Validate(const Array<Vec2>& vertices, const Array<Array<Vec2>>& holes)
	{
		return Validate(vertices.data(), vertices.size(), holes);
	}

	inline const Polygon::PolygonDetail* Polygon::_detail() const noexcept
	{
		return pImpl.get();
	}

	inline void swap(Polygon& a, Polygon& b) noexcept
	{
		a.swap(b);
	}
}

template <>
void std::swap(s3d::Polygon& a, s3d::Polygon& b) noexcept
{
	a.swap(b);
}