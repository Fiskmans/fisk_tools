
#ifndef FISK_TOOLS_MATRIX_H
#define FISK_TOOLS_MATRIX_H

#include "tools/Utility.h"
#include "tools/MathVector.h"

#include <array>
#include <functional>

namespace fisk::tools
{
	template<typename Type, size_t Columns, class DimensionSequence>
	class Matrix_impl;

	template<typename Type, size_t Columns, size_t... IndexSequence>
	class Matrix_impl<Type, Columns, std::index_sequence<IndexSequence...>>
	{
	public:
		static_assert(sizeof...(IndexSequence) % Columns == 0, "Do not instantiate this class manually, use the Matrix template instead");
		static constexpr size_t Rows = sizeof...(IndexSequence) / Columns;
		static constexpr bool Symmetric = Rows == Columns;

		Matrix_impl() = default;
		Matrix_impl(ExtractTypeFromUnpackedNumber<Type, IndexSequence>... aValue);

		MathVector<Type, Columns> GetRow(size_t aRow) const;
		MathVector<Type, sizeof...(IndexSequence) / Columns> GetColumn(size_t aColumn) const;

		Type& GetElement(size_t aColumn, size_t aRow);
		Type* Raw();

		template <size_t OtherColumns> // OtherRows = Columns
		Matrix_impl<Type, OtherColumns, std::make_index_sequence<OtherColumns * sizeof...(IndexSequence) / Columns>>
		operator*(const Matrix_impl<Type, OtherColumns, std::make_index_sequence<Columns * OtherColumns>>& aOther);

		Matrix_impl& operator*=(const Matrix_impl& aOther);


	private:
		MathVector<Type, Columns> myRows[Rows];
	};
	
	template <typename Type, size_t Columns, size_t... IndexSequence>
	inline Matrix_impl<Type, Columns, std::index_sequence<IndexSequence...>>
		::Matrix_impl(
		ExtractTypeFromUnpackedNumber<Type, IndexSequence>... aValue)
	{
		((myRows[IndexSequence / Columns][IndexSequence % Columns] = aValue), ...);
	}

	template <typename Type, size_t Columns, size_t... IndexSequence>
	inline MathVector<Type, Columns> 
		Matrix_impl<Type, Columns, std::index_sequence<IndexSequence...>>
		::GetRow(size_t aRow) const
	{
		assert(aRow < Rows);

		return myRows[aRow];
	}

	template <typename Type, size_t Columns, size_t... IndexSequence>
	inline MathVector<Type, sizeof...(IndexSequence) / Columns> 
		Matrix_impl<Type, Columns, std::index_sequence<IndexSequence...>>
		::GetColumn(size_t aColumn) const
	{
		assert(aColumn < Columns);

		MathVector<Type, Rows> out;

		for (size_t i = 0; i < Rows; i++)
			out[i] = myRows[i][aColumn];

		return out;
	}

	template <typename Type, size_t Columns, size_t... IndexSequence>
	inline Type& 
		Matrix_impl<Type, Columns, std::index_sequence<IndexSequence...>>
		::GetElement(size_t aColumn, size_t aRow)
	{
		assert(aRow < Rows);
		assert(aColumn < Columns);

		return myRows[aRow][aColumn];
	}

	template <typename Type, size_t Columns, size_t... IndexSequence>
	inline Type* Matrix_impl<Type, Columns, std::index_sequence<IndexSequence...>>::Raw()
	{
		static_assert(sizeof(myRows) == sizeof(*myRows) * Rows, "The raw elements are not packed as expected");
		return myRows[0].Raw();
	}

	template <typename Type, size_t Columns, size_t... IndexSequence>
	template <size_t OtherColumns>
	inline Matrix_impl<Type, OtherColumns, std::make_index_sequence<OtherColumns * sizeof...(IndexSequence) / Columns>>
	Matrix_impl<Type, Columns, std::index_sequence<IndexSequence...>>::operator*(
		const Matrix_impl<Type, OtherColumns, std::make_index_sequence<Columns * OtherColumns>>& aOther)
	{
		constexpr size_t resultRows	   = Rows;
		constexpr size_t resultColumns = OtherColumns;

		Matrix_impl<Type, resultColumns, std::make_index_sequence<resultColumns * resultRows>> out;

		std::array<MathVector<Type, Columns>, resultRows> rows;
		std::array<MathVector<Type, Columns>, resultColumns> columns;

		for (size_t i = 0; i < resultRows; i++)
			rows[i] = GetRow(i);
		
		for (size_t i = 0; i < resultColumns; i++)
			columns[i] = aOther.GetColumn(i);

		for (size_t y = 0; y < resultRows; y++)
			for (size_t x = 0; x < resultColumns; x++)
				out.GetElement(x, y) = columns[x].Dot(rows[y]);

		return out;
	}

	template <typename Type, size_t Columns, size_t... IndexSequence>
	inline Matrix_impl<Type, Columns, std::index_sequence<IndexSequence...>>& Matrix_impl<Type, Columns, std::index_sequence<IndexSequence...>>::operator*=(
		const Matrix_impl& aOther)
	{
		//static_assert(Symmetric, "This operation is only available on symmetric matrixes");

		*this = *this * aOther;
		return *this;
	}

	template <typename Type, size_t Columns, size_t Rows>
	using Matrix = Matrix_impl<Type, Columns, std::make_index_sequence<Rows * Columns>>;

} // namespace fisk::tools

#endif