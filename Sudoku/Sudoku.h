#pragma once

#include "Includes.h"

class Sudoku
{
	int Cells[9][9]; // accessed with Cells[Row][Column]
	int Candidates[9][9][9]; // accessed with Candidates[Row][Column][0 - 8]

	bool PrefilledCells[81];
	bool IncorrectCells[81];

	int SelectedCell = -1;
	bool bInputDisabled = false;

public:

	Sudoku();

	void Import(const char* CellsStr);
	char* Export() const;

	bool IsSolved() const;

private:

	void Reset();

	// this doesn't follow the const correctness but it's more convenient this way
	// for the predicate: return true if we should move on to the next cell, return false if we should stop and return
	void IterateCells(std::function<bool(const ImVec2& CellMin, const ImVec2& CellMax, int Row, int Column)> Predicate) const;

	void UpdateIncorrectRows();
	void UpdateIncorrectColumns();
	void UpdateIncorrectBlocks();
	void UpdateIncorrectCells();

	void SolveStep(int Cell);

	void SelectCell();
	void HandleInput();

	int CountFilledCells() const;
	int CountIncorrectCells() const;

	const ImVec2 GetCellSize() const;

	std::pair<ImVec2, ImVec2> GetCell(std::pair<int, int> RowColumn) const;
	std::pair<ImVec2, ImVec2> GetCell(int Cell) const;

	const ImU32 GetCellColor(int Cell, bool bSelected, bool bDirectlySelected, bool bIsSameValue) const;
	const ImU32 GetCellColor(std::pair<int, int> RowColumn, bool bSelected, bool bDirectlySelected, bool bIsSameValue) const;

	void DrawCandiates(int Cell) const;
	void DrawCellValue(int CellValue, const ImVec2& CellMin, const ImVec2& CellMax) const;

	void DrawGrid() const;
	void DrawCells() const;
	void DrawSelectedCell() const;
	void DrawIncorrectCells() const;

public:

	void Update(const float DeltaTime);
	void Draw(const float DeltaTime) const;
};