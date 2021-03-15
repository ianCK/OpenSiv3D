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
# include "Common.hpp"
# include "Array.hpp"
# include "WaveSample.hpp"

namespace s3d
{
	class Wave
	{
	public:

		using base_type					= Array<WaveSample>;
		using allocator_type			= base_type::allocator_type;
		using value_type				= base_type::value_type;
		using size_type					= base_type::size_type;
		using difference_type			= base_type::difference_type;
		using pointer					= base_type::pointer;
		using const_pointer				= base_type::const_pointer;
		using reference					= base_type::reference;
		using const_reference			= base_type::const_reference;
		using iterator					= base_type::iterator;
		using const_iterator			= base_type::const_iterator;
		using reverse_iterator			= base_type::reverse_iterator;
		using const_reverse_iterator	= base_type::const_reverse_iterator;

		static constexpr uint32 MinSamplingRate		= 4000;

		static constexpr uint32 DefaultSamplingRate	= 44100;

		static constexpr uint32 MaxSamplingRate		= 192000;

		SIV3D_NODISCARD_CXX20
		Wave() = default;

		SIV3D_NODISCARD_CXX20
		explicit Wave(const allocator_type& alloc) noexcept;

		SIV3D_NODISCARD_CXX20
		Wave(size_t count, const value_type& value, const allocator_type& alloc = allocator_type{});

		SIV3D_NODISCARD_CXX20
		explicit Wave(size_t count, const allocator_type& alloc = allocator_type{});

		template <class Iterator>
		Wave(Iterator first, Iterator last, const allocator_type& alloc = allocator_type{});

		SIV3D_NODISCARD_CXX20
		Wave(const Wave& samples);

		SIV3D_NODISCARD_CXX20
		Wave(const Wave& samples, const allocator_type& alloc);

		SIV3D_NODISCARD_CXX20
		Wave(Wave&& samples) noexcept;

		SIV3D_NODISCARD_CXX20
		Wave(std::initializer_list<value_type> init, const allocator_type& alloc = allocator_type{});

		SIV3D_NODISCARD_CXX20
		explicit Wave(const Array<value_type>& samples);

		SIV3D_NODISCARD_CXX20
		explicit Wave(Array<value_type>&& samples) noexcept;

		SIV3D_NODISCARD_CXX20
		explicit Wave(Arg::reserve_<size_type> size);

		Wave& operator =(const Array<value_type>& other);

		Wave& operator =(Array<value_type>&& other) noexcept;

		Wave& operator =(const Wave& other);

		Wave& operator =(Wave&& other) noexcept;

		[[nodiscard]]
		const Array<value_type>& asArray() const noexcept;

		[[nodiscard]]
		operator const Array<value_type>& () const noexcept;

		[[nodiscard]]
		uint32 samplingRate() const noexcept;

		void setSamplingRate(uint32 samplingRate) noexcept;

		[[nodiscard]]
		size_t samples() const noexcept;

		[[nodiscard]]
		size_t lengthSample() const noexcept;

		[[nodiscard]]
		double lengthSec() const noexcept;

		void fillZero();

		//bool save(const FilePath& path, AudioFormat format = AudioFormat::WAVE);

		//bool saveWAVE(FilePathView path, WAVEFormat format = WAVEFormat::Default);

		//bool saveOggVorbis(FilePathView path, int32 quality = 60);

		template <class Iterator>
		void assign(Iterator first, Iterator last);

		void assign(size_type n, const value_type& value);

		void assign(std::initializer_list<value_type> il);

		void assign(const Array<value_type>& other);

		void assign(Array<value_type>&& other);

		void assign(const Wave& other);

		void assign(Wave&& other) noexcept;

		[[nodiscard]]
		allocator_type get_allocator() const noexcept;

		[[nodiscard]]
		value_type& at(size_t index)&;

		[[nodiscard]]
		const value_type& at(size_t index) const&;

		[[nodiscard]]
		value_type at(size_t index)&&;

		[[nodiscard]]
		value_type& operator[](size_t index) & noexcept;

		[[nodiscard]]
		const value_type& operator[](size_t index) const& noexcept;

		[[nodiscard]]
		value_type operator[](size_t index) && noexcept;

		void push_front(const value_type & value);

		void push_back(const value_type & value);

		void pop_front();

		void pop_front_N(size_t n);

		void pop_back() noexcept;

		void pop_back_N(size_t n);

		Wave& operator <<(const value_type& value);

		template <class... Args>
		iterator emplace(const_iterator position, Args&&... args);

		template <class... Args>
		decltype(auto) emplace_back(Args&&... args);

		[[nodiscard]]
		value_type& front() noexcept;

		[[nodiscard]]
		const value_type& front() const noexcept;

		[[nodiscard]]
		value_type& back() noexcept;

		[[nodiscard]]
		const value_type& back() const noexcept;

		void swap(Wave& other) noexcept;

		[[nodiscard]]
		const value_type* data() const noexcept;

		[[nodiscard]]
		value_type* data() noexcept;

		[[nodiscard]]
		iterator begin() noexcept;

		[[nodiscard]]
		iterator end() noexcept;

		[[nodiscard]]
		const_iterator begin() const noexcept;

		[[nodiscard]]
		const_iterator end() const noexcept;

		[[nodiscard]]
		const_iterator cbegin() const noexcept;

		[[nodiscard]]
		const_iterator cend() const noexcept;

		[[nodiscard]]
		reverse_iterator rbegin() noexcept;

		[[nodiscard]]
		reverse_iterator rend() noexcept;

		[[nodiscard]]
		const_reverse_iterator rbegin() const noexcept;

		[[nodiscard]]
		const_reverse_iterator rend() const noexcept;

		[[nodiscard]]
		const_reverse_iterator crbegin() const noexcept;

		[[nodiscard]]
		const_reverse_iterator crend() const noexcept;

		[[nodiscard]]
		bool empty() const noexcept;

		[[nodiscard]]
		bool isEmpty() const noexcept;

		[[nodiscard]]
		explicit operator bool() const noexcept;

		[[nodiscard]]
		size_t size_bytes() const noexcept;

		[[nodiscard]]
		size_t size() const noexcept;

		[[nodiscard]]
		size_t max_size() const noexcept;

		void reserve(size_t newCapacity);

		[[nodiscard]]
		size_t capacity() const noexcept;

		void shrink_to_fit();

		void clear() noexcept;

		void release();

		iterator insert(const_iterator where, const value_type& value);

		iterator insert(const_iterator where, size_t count, const value_type& value);

		template <class Iterator>
		iterator insert(const_iterator where, Iterator first, Iterator last);

		iterator insert(const_iterator where, std::initializer_list<value_type> il);

		iterator erase(const_iterator where) noexcept;

		iterator erase(const_iterator first, const_iterator last) noexcept;

		void resize(size_t newSize);

		void resize(size_t newSize, const value_type& value);

		template <class Fty = decltype(Identity), std::enable_if_t<std::is_invocable_r_v<bool, Fty, value_type>>* = nullptr>
		[[nodiscard]]
		bool all(Fty f = Identity) const;

		template <class Fty = decltype(Identity), std::enable_if_t<std::is_invocable_r_v<bool, Fty, value_type>>* = nullptr>
		[[nodiscard]]
		bool any(Fty f = Identity) const;

		[[nodiscard]]
		size_t count(const value_type& value) const;

		template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty, value_type>>* = nullptr>
		[[nodiscard]]
		size_t count_if(Fty f) const;

		Wave& fill(const value_type& value);

		template <class Fty = decltype(Identity), std::enable_if_t<std::is_invocable_r_v<bool, Fty, value_type>>* = nullptr>
		[[nodiscard]]
		bool none(Fty f = Identity) const;

		Wave& append(const Array<value_type>&other);

		Wave& append(const Wave& other);

		Wave& remove(const value_type& value);

		Wave& remove_at(size_t index);

		template <class Fty>
		Wave& remove_if(Fty f);

		Wave& reverse();

		[[nodiscard]]
		Wave reversed() const;

		[[nodiscard]]
		Wave slice(size_t index) const;

		[[nodiscard]]
		Wave slice(size_t index, size_t length) const;

	private:

		base_type m_data;

		uint32 m_samplingRate = DefaultSamplingRate;
	};
}

# include "detail/Wave.ipp"