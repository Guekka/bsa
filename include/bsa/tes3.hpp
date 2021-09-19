#pragma once

#include <compare>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <utility>

#include <binary_io/any_stream.hpp>

#include "bsa/detail/common.hpp"

namespace bsa::tes3
{
#ifndef DOXYGEN
	namespace detail
	{
		using namespace bsa::detail;
	}
#endif

	namespace hashing
	{
		/// \brief	The underlying hash object used to uniquely identify objects within the archive.
		struct hash final
		{
		public:
			std::uint32_t lo{ 0 };
			std::uint32_t hi{ 0 };

			[[nodiscard]] friend bool operator==(const hash&, const hash&) noexcept = default;

			[[nodiscard]] friend std::strong_ordering operator<=>(
				const hash& a_lhs,
				const hash& a_rhs) noexcept
			{
				return a_lhs.numeric() <=> a_rhs.numeric();
			}

			/// \brief	Obtains the numeric value of the hash used for comparisons.
			[[nodiscard]] std::uint64_t numeric() const noexcept
			{
				return std::uint64_t{
					std::uint64_t{ hi } << 0u * 8u |
					std::uint64_t{ lo } << 4u * 8u
				};
			}

#ifndef DOXYGEN
			friend auto operator>>(
				detail::istream_t& a_in,
				hash& a_hash)
				-> detail::istream_t&;

			friend auto operator<<(
				detail::ostream_t& a_out,
				const hash& a_hash) noexcept
				-> detail::ostream_t&;
#endif
		};

		/// \brief	Produces a hash using the given path.
		/// \remark	The path is normalized in place. After the function returns,
		///		the path contains the string that would be stored on disk.
		[[nodiscard]] hash hash_file_in_place(std::string& a_path) noexcept;

		/// \copybrief	hash_file_in_place()
		/// \remark	See also \ref bsa::concepts::stringable.
		template <concepts::stringable String>
		[[nodiscard]] hash hash_file(String&& a_path) noexcept
		{
			std::string str(std::forward<String>(a_path));
			return hash_file_in_place(str);
		}
	}

	/// \brief	Represents a file within the TES:3 virtual filesystem.
	class file final :
		public components::byte_container
	{
	private:
		friend archive;
		using super = components::byte_container;

	public:
		/// \brief	The key used to indentify a file.
		using key = components::key<hashing::hash, hashing::hash_file_in_place>;

		/// \brief	Clears the contents of the file.
		using super::clear;
	};

	/// \brief	Represents the TES:3 revision of the bsa format.
	class archive final :
		public components::hashmap<file>
	{
	private:
		using super = components::hashmap<file>;

	public:
#ifdef DOXYGEN
		/// \brief	Clears the contents of the archive.
		void clear() noexcept;
#else
		using super::clear;
#endif

		/// \copydoc bsa::tes3::archive::read()
		///
		/// \exception	std::system_error	Thrown when filesystem errors are encountered.
		///
		/// \remark	If `std::system_error` is thrown, the archive is left unmodified.
		///
		/// \param	a_path	The path to the given archive on the native filesystem.
		void read(std::filesystem::path a_path);

		/// \copydoc bsa::tes3::archive::read()
		///
		/// \param	a_src	The source to read from.
		void read(std::span<const std::byte> a_src);

		/// \brief	Verifies that offsets within the archive will be valid when written to disk.
		///
		/// \return	Returns `true` is the archive passes validation, `false` otherwise.
		[[nodiscard]] bool verify_offsets() const noexcept;

		/// \copydoc bsa::tes3::archive::write() const
		///
		/// \param	a_path	The path to write the archive to on the native filesystem.
		void write(std::filesystem::path a_path) const;

		/// \copydoc bsa::tes3::archive::write() const
		///
		/// \param	a_dst	The stream to write the archive to.
		void write(binary_io::any_ostream& a_dst) const;

#ifdef DOXYGEN
	protected:
		/// \brief	Reads the contents of the archive from disk.
		///
		/// \exception	binary_io::buffer_exhausted	Thrown when archive reads index out of bounds.
		/// \exception	bsa::exception	Thrown when archive parsing errors are encountered.
		///
		///	\remark	If `binary_io::buffer_exhausted` or `bsa::exception` is thrown, the archive is
		///		left in an unspecified state. Use \ref clear to return it to a valid state.
		/// \remark	If the function returns successfully, the contents of the archived are replaced
		///		with the contents of the archive on disk.
		void read();

		/// \brief	Writes the contents of the archive to disk.
		///
		/// \exception	std::system_error	Thrown when filesystem errors are encountered.
		/// \exception	binary_io::buffer_exhausted	Thrown when the underlying buffer is exhausted.
		void write() const;
#endif

	private:
		struct offsets_t;

		void do_read(detail::istream_t& a_in);
		void do_write(detail::ostream_t& a_out) const;

		[[nodiscard]] auto make_header() const noexcept -> detail::header_t;

		void read_file(
			detail::istream_t& a_in,
			const offsets_t& a_offsets,
			std::size_t a_idx);

		void write_file_entries(detail::ostream_t& a_out) const noexcept;
		void write_file_name_offsets(detail::ostream_t& a_out) const noexcept;
		void write_file_names(detail::ostream_t& a_out) const noexcept;
		void write_file_hashes(detail::ostream_t& a_out) const noexcept;
		void write_file_data(detail::ostream_t& a_out) const noexcept;
	};
}
