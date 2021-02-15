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
	inline MultiPolygon::MultiPolygon(const MultiPolygon& polygons)
		: base_type{ polygons } {}

	inline MultiPolygon::MultiPolygon(MultiPolygon&& polygons) noexcept
		: base_type(std::move(polygons)) {}

	inline MultiPolygon::MultiPolygon(const Array<Polygon>& polygons)
		: base_type{ polygons } {}

	inline MultiPolygon::MultiPolygon(Array<Polygon>&& polygons) noexcept
		: base_type(std::move(polygons)) {}

	inline MultiPolygon& MultiPolygon::operator =(const Array<Polygon>& other)
	{
		base_type::operator =(other);

		return *this;
	}

	inline MultiPolygon& MultiPolygon::operator =(Array<Polygon>&& other) noexcept
	{
		base_type::operator =(std::move(other));

		return *this;
	}

	inline MultiPolygon& MultiPolygon::operator =(const MultiPolygon& other)
	{
		base_type::operator =(other);

		return *this;
	}

	inline MultiPolygon& MultiPolygon::operator =(MultiPolygon&& other) noexcept
	{
		base_type::operator =(std::move(other));

		return *this;
	}

	inline void MultiPolygon::assign(const MultiPolygon& other)
	{
		base_type::operator =(other);
	}

	inline void MultiPolygon::assign(MultiPolygon&& other) noexcept
	{
		base_type::operator =(std::move(other));
	}

	inline MultiPolygon& MultiPolygon::operator <<(const Polygon& value)
	{
		base_type::push_back(value);

		return *this;
	}

	inline void MultiPolygon::swap(MultiPolygon& other) noexcept
	{
		base_type::swap(other);
	}

	inline MultiPolygon& MultiPolygon::append(const Array<Polygon>& other)
	{
		base_type::insert(end(), other.begin(), other.end());

		return *this;
	}

	inline MultiPolygon& MultiPolygon::append(const MultiPolygon& other)
	{
		base_type::insert(end(), other.begin(), other.end());

		return *this;
	}

	inline MultiPolygon& MultiPolygon::remove_at(const size_t index)
	{
		base_type::remove_at(index);

		return *this;
	}

	template <class Fty>
	inline MultiPolygon& MultiPolygon::remove_if(Fty f)
	{
		base_type::remove_if(f);

		return *this;
	}

	inline MultiPolygon& MultiPolygon::reverse()
	{
		base_type::reverse();

		return *this;
	}

	inline MultiPolygon MultiPolygon::reversed() const
	{
		return MultiPolygon(rbegin(), rend());
	}

	inline MultiPolygon& MultiPolygon::shuffle()
	{
		base_type::shuffle();

		return *this;
	}

	SIV3D_CONCEPT_URBG_
	inline MultiPolygon& MultiPolygon::shuffle(URBG&& rbg)
	{
		base_type::shuffle(std::forward<URBG>(rbg));

		return *this;
	}

	inline MultiPolygon MultiPolygon::slice(const size_t index) const
	{
		if (index >= size())
		{
			return{};
		}

		return MultiPolygon(begin() + index, end());
	}

	inline MultiPolygon MultiPolygon::slice(const size_t index, const size_t length) const
	{
		if (index >= size())
		{
			return{};
		}

		return MultiPolygon(begin() + index, begin() + Min(index + length, size()));
	}

	inline MultiPolygon MultiPolygon::movedBy(const double x, const double y) const
	{
		MultiPolygon polygons{ *this };

		polygons.moveBy(x, y);

		return polygons;
	}

	inline MultiPolygon MultiPolygon::movedBy(const Vec2 v) const
	{
		return movedBy(v.x, v.y);
	}

	inline MultiPolygon& MultiPolygon::moveBy(const double x, const double y) noexcept
	{
		for (auto& polygon : *this)
		{
			polygon.moveBy(x, y);
		}

		return *this;
	}

	inline MultiPolygon& MultiPolygon::moveBy(const Vec2 v) noexcept
	{
		return moveBy(v.x, v.y);
	}

	inline MultiPolygon MultiPolygon::rotated(const double angle) const
	{
		return MultiPolygon{ map([=](const Polygon& p) { return p.rotated(angle); }) };
	}

	inline MultiPolygon MultiPolygon::rotatedAt(const Vec2& pos, const double angle) const
	{
		return MultiPolygon{ map([=](const Polygon& p) { return p.rotatedAt(pos, angle); }) };
	}

	inline MultiPolygon& MultiPolygon::rotate(const double angle)
	{
		for (auto& polygon : *this)
		{
			polygon.rotate(angle);
		}

		return *this;
	}

	inline MultiPolygon& MultiPolygon::rotateAt(const Vec2& pos, const double angle)
	{
		for (auto& polygon : *this)
		{
			polygon.rotateAt(pos, angle);
		}

		return *this;
	}

	inline MultiPolygon MultiPolygon::transformed(const double s, double c, const Vec2& pos) const
	{
		return MultiPolygon{ map([=](const Polygon& p) { return p.transformed(s, c, pos); }) };
	}

	inline MultiPolygon& MultiPolygon::transform(const double s, const double c, const Vec2& pos)
	{
		for (auto& polygon : *this)
		{
			polygon.transform(s, c, pos);
		}

		return *this;
	}

	inline MultiPolygon MultiPolygon::scaled(const double s) const
	{
		return MultiPolygon{ map([=](const Polygon& p) { return p.scaled(s); }) };
	}

	inline MultiPolygon& MultiPolygon::scale(const double s)
	{
		for (auto& polygon : *this)
		{
			polygon.scale(s);
		}

		return *this;
	}

	inline MultiPolygon MultiPolygon::simplified(const double maxDistance) const
	{
		return MultiPolygon{ map([=](const Polygon& p) { return p.simplified(maxDistance); }) };
	}

	template <class Shape2DType>
	inline bool MultiPolygon::intersects(const Shape2DType& other) const
	{
		return Geometry2D::Intersect(*this, other);
	}

	template <class Shape2DType>
	inline Optional<Array<Vec2>> MultiPolygon::intersectsAt(const Shape2DType& other) const
	{
		return Geometry2D::IntersectAt(*this, other);
	}

	template <class Shape2DType>
	inline bool MultiPolygon::contains(const Shape2DType& other) const
	{
		return Geometry2D::Contains(*this, other);
	}
}

template <>
void std::swap(s3d::MultiPolygon& a, s3d::MultiPolygon& b) noexcept
{
	a.swap(b);
}