#pragma once

#include <utility>

namespace Helper
{
	constexpr int RowColumnToCell(std::pair<int, int> RowColumn);
	constexpr std::pair<int, int> CellToRowColumn(int Cell);

	constexpr int CellToBlock(int Cell);
	constexpr int BlockToCell(int Block);
}

constexpr int Helper::RowColumnToCell(std::pair<int, int> RowColumn)
{
	const auto [Row, Column] = RowColumn;

	return Row * 9 + Column;
}

constexpr std::pair<int, int> Helper::CellToRowColumn(int Cell)
{
	int Row = Cell / 9;
	int Column = Cell % 9;

	return { Row, Column };
}

constexpr int Helper::CellToBlock(int Cell)
{
	auto [Row, Column] = CellToRowColumn(Cell);

	Row /= 3;
	Column /= 3;

	return Row * 3 + Column;
}

constexpr int Helper::BlockToCell(int Block)
{
	int Row = Block / 3;
	int Column = Block % 3;

	Row *= 3;
	Column *= 3;

	return RowColumnToCell({ Row, Column });
}
