#include "Sudoku.h"

#include "Helper.h"

#define PADDING				(ImVec2(10.f, 10.f))

#define PADDING_THICKNESS	(ImVec2(12.5f, 12.5f))
#define GRID_THICKNESS		(ImVec2(2.5f, 2.5f))
#define BLOCK_THICKNESS		(ImVec2(7.5f, 7.5f))

#define PADDING_COL										IM_COL32_BLACK
#define BG_COL											IM_COL32_WHITE
#define GRID_COL										IM_COL32(151, 151, 151, 255)
#define TEXT_COL										IM_COL32(18, 18, 18, 255)
#define INCORRECT_DOT_COL								IM_COL32(255, 75, 86, 255)
#define DEFAULT_CELL_COL								IM_COL32_WHITE
#define PREFILLED_CELL_COL								IM_COL32(223, 223, 223, 255)
#define DIRECTLY_SELECTED_COL							IM_COL32(251, 155, 0, 255)
#define INDIRECTLY_SELECTED_COL							IM_COL32(249, 234, 194, 255)
#define PREFILLED_DIRECTLY_SELECTED_COL					IM_COL32(212, 130, 0, 255)
#define PREFILLED_INDIRECTLY_SELECTED_COL				IM_COL32(211, 198, 175, 255)
#define SAME_VALUE_INDIRECTLY_SELECTED_COL				IM_COL32(254, 196, 104, 255)
#define PREFILLED_SAME_VALUE_INDIRECTLY_SELECTED_COL	IM_COL32(230, 145, 0, 255)

constexpr int BlockOffsets[] = { 0, 1, 2, 9, 10, 11, 18, 19, 20 };

constexpr const char* SolveMessages[] = {
	"CONGRATS0YOU000000SOLVED000THE000000SUDOKU000WELL00000DONE00000PLAYER000AMAZING00",
	"WELL00000DONE00000YOU000000COMPLETEDTHE000000SUDOKU000PUZZLE000PERFECTLYGREAT0000",
	"YOU000000ARE000000AN0000000AMAZING00SUDOKU000SOLVER000GREAT0000LOGIC0000SKILLS000",
	"CONGRATS0YOU000000HAVE00000MASTERED0THE000000SUDOKU000BOARD0000EXCELLENTJOB000000",
	"YOU000000SOLVED000THIS00000SUDOKU000WITH00000GREAT0000ACCURACY0AND000000SKILL0000",
	"WELL00000DONE00000YOU000000CRACKED00THE000000SUDOKU000PUZZLE000WITH00000EASE00000",
	"CONGRATS0YOU000000COMPLETEDTHE000000SUDOKU000GRID00000PERFECTLYWELL00000DONE00000",
	"YOU000000SHOWED000GREAT0000LOGIC0000WHILE0000SOLVING00THE000000SUDOKU000PUZZLE000",
	"EXCELLENTWORK00000YOU000000FINISHED0THE000000SUDOKU000BOARD0000WITH00000CARE00000",
	"YOU000000ARE000000TRULY0000A00000000SUDOKU000MASTER000WELL00000DONE00000AMAZING0"
};

static int SolveMessageIdx = 0;

Sudoku::Sudoku()
{
	Reset();
}

void Sudoku::Import(const char* CellsStr)
{
	Reset();

	int Idx = 0;

	for (int Column = 0; Column < 9; Column++)
	{
		for (int Row = 0; Row < 9; Row++)
		{
			int Cell = Helper::RowColumnToCell({ Row, Column });
			int CellValue = CellsStr[Idx++] - '0';

			if (CellValue == ('.' - '0')) // allow dots as zeroes
			{
				CellValue = 0;
			}

			Cells[Row][Column] = CellValue;

			if (CellValue != 0)
			{
				PrefilledCells[Cell] = true;
			}
		}
	}
}

char* Sudoku::Export() const
{
	char* Out = new char[81 + 1];
	int Idx = 0;

	for (int Column = 0; Column < 9; Column++)
	{
		for (int Row = 0; Row < 9; Row++)
		{
			Out[Idx++] = '0' + Cells[Row][Column];
		}
	}

	Out[Idx] = '\0';

	return Out;
}

bool Sudoku::IsSolved() const
{
	bool bAreAllSquaresFilled = true;

	IterateCells([this, &bAreAllSquaresFilled](const ImVec2& CellMin, const ImVec2& CellMax, int Row, int Column) -> bool
		{
			if (Cells[Row][Column] == 0)
			{
				bAreAllSquaresFilled = false;
				return false;
			}

			return true;
		});

	if (!bAreAllSquaresFilled)
	{
		return false;
	}

	for (bool bIncorrectCell : IncorrectCells)
	{
		if (bIncorrectCell)
		{
			return false;
		}
	}

	return true;
}

void Sudoku::Reset()
{
	memset(Cells, 0, sizeof(Cells));
	memset(Candidates, 0, sizeof(Candidates));
	memset(PrefilledCells, 0, sizeof(PrefilledCells));
	memset(IncorrectCells, 0, sizeof(IncorrectCells));
}

void Sudoku::IterateCells(std::function<bool(const ImVec2& CellMin, const ImVec2& CellMax, int Row, int Column)> Predicate) const
{
	const ImVec2 Size = ImGui::GetWindowSize();

	const ImVec2 Center = Size / 2.f;
	const ImVec2 CellSize = GetCellSize();

	for (int Row = 0; Row < 9; Row++)
	{
		for (int Column = 0; Column < 9; Column++)
		{
			const float x = Center.x + (Row - 4.5f) * CellSize.x;
			const float y = Center.y + (Column - 4.5f) * CellSize.y;

			const ImVec2 CellMin = { x, y };
			const ImVec2 CellMax = CellMin + CellSize;

			if (!Predicate(CellMin, CellMax, Row, Column))
			{
				return;
			}
		}
	}
}

void Sudoku::UpdateIncorrectRows()
{
	for (int Row = 0; Row < 9; Row++)
	{
		std::unordered_map<int, std::vector<int>> Occurrences; // cell value -> vector of cells

		for (int Column = 0; Column < 9; Column++)
		{
			int CurrentCell = Helper::RowColumnToCell({ Row, Column });
			int CurrentCellValue = Cells[Row][Column];

			if (CurrentCellValue == 0)
			{
				continue;
			}

			Occurrences[CurrentCellValue].push_back(CurrentCell);
		}

		for (const auto& [CellValue, Cells] : Occurrences)
		{
			if (Cells.size() > 1)
			{
				for (int Cell : Cells)
				{
					IncorrectCells[Cell] = true;
				}
			}
		}
	}
}

void Sudoku::UpdateIncorrectColumns()
{
	for (int Column = 0; Column < 9; Column++)
	{
		std::unordered_map<int, std::vector<int>> Occurrences; // cell value -> vector of cells

		for (int Row = 0; Row < 9; Row++)
		{
			int CurrentCell = Helper::RowColumnToCell({ Row, Column }); 
			int CurrentCellValue = Cells[Row][Column];

			if (CurrentCellValue == 0)
			{
				continue;
			}

			Occurrences[CurrentCellValue].push_back(CurrentCell);
		}

		for (const auto& [CellValue, Cells] : Occurrences)
		{
			if (Cells.size() > 1)
			{
				for (int Cell : Cells)
				{
					IncorrectCells[Cell] = true;
				}
			}
		}
	}
}

void Sudoku::UpdateIncorrectBlocks()
{
	for (int Block = 0; Block < 9; Block++)
	{
		std::unordered_map<int, std::vector<int>> Occurrences; // cell value -> vector of cells

		int StartingCell = Helper::BlockToCell(Block);

		for (int Offset : BlockOffsets)
		{
			int CurrentCell = StartingCell + Offset;

			const auto [Row, Column] = Helper::CellToRowColumn(CurrentCell);

			int CurrentCellValue = Cells[Row][Column];

			if (CurrentCellValue == 0)
			{
				continue;
			}

			Occurrences[CurrentCellValue].push_back(CurrentCell);
		}

		for (const auto& [CellValue, Cells] : Occurrences)
		{
			if (Cells.size() > 1)
			{
				for (int Cell : Cells)
				{
					IncorrectCells[Cell] = true;
				}
			}
		}
	}
}

void Sudoku::UpdateIncorrectCells()
{
	// updating every cell every time any cell is update is not exactly the best solution, but it's efficient enough for our purposes

	memset(IncorrectCells, false, sizeof(IncorrectCells));

	UpdateIncorrectRows();
	UpdateIncorrectColumns();
	UpdateIncorrectBlocks();
}

void Sudoku::SolveStep(int Cell)
{
	if (PrefilledCells[Cell])
	{
		return;
	}

	if (CountFilledCells() < 81)
	{
		const auto [Row, Column] = Helper::CellToRowColumn(Cell);

		int CellValue = Cells[Row][Column];

		if (CellValue == 0 || IncorrectCells[Cell])
		{
			for (int i = 1; i <= 9; i++)
			{
				Cells[Row][Column] = i;
				UpdateIncorrectCells();

				if (!IncorrectCells[Cell])
				{
					break;
				}
			}
		}
	}
	else
	{
		// shuffle the incorrect squares

		int NumIncorrectCellsBefore = CountIncorrectCells();
		int NumUnfilledCellsBefore = 81 - CountFilledCells();
		int NumUnfinishedCellsBefore = NumIncorrectCellsBefore + NumUnfilledCellsBefore;

		if (NumUnfinishedCellsBefore > 0)
		{
			int IncorrectCell = -1;

			for (int CurrentCell = 0; CurrentCell < 81; CurrentCell++)
			{
				if (IncorrectCells[CurrentCell])
				{
					IncorrectCell = CurrentCell;
					break;
				}
			}

			if (IncorrectCell == -1)
			{
				return;
			}

			const auto [Row, Column] = Helper::CellToRowColumn(IncorrectCell);

			int LeastConflicts = INT32_MAX;
			int LeastConflictingValue = 0;

			for (int i = 1; i <= 9; i++)
			{
				Cells[Row][Column] = i;
				UpdateIncorrectCells();

				int Conflicts = CountIncorrectCells();

				if (Conflicts < LeastConflicts)
				{
					LeastConflicts = Conflicts;
					LeastConflictingValue = i;
				}
			}

			Cells[Row][Column] = LeastConflictingValue;
		}
	}
}

void Sudoku::SelectCell()
{
	const ImVec2 MousePos = ImGui::GetMousePos();

	IterateCells([MousePos, this](const ImVec2& CellMin, const ImVec2& CellMax, int Row, int Column) -> bool
		{
			if (MousePos >= CellMin && MousePos <= CellMax)
			{
				SelectedCell = Helper::RowColumnToCell({ Row, Column });
				return false;
			}

			return true;
		});
}

void Sudoku::HandleInput()
{
	const bool bLeftMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);	
	const bool bRightMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Right);

	if (bLeftMouseClicked)
	{
		SelectCell();
	}
	else if (bRightMouseClicked)
	{
		SelectedCell = -1;
	}

	if (!bInputDisabled)
	{
		if (SelectedCell != -1)
		{
			const bool bIsAltHeld = ImGui::IsKeyDown(ImGuiMod_Alt);

			const auto [Row, Column] = Helper::CellToRowColumn(SelectedCell);
			const int CurrentCellValue = Cells[Row][Column];

			if (!PrefilledCells[SelectedCell])
			{
				for (int i = 1; i <= 9; i++)
				{
					if (ImGui::Shortcut(ImGuiKey_0 + i))
					{
						if (CurrentCellValue != i)
						{
							Cells[Row][Column] = i;
							UpdateIncorrectCells();
						}
					}
				}

				if (CurrentCellValue != 0)
				{
					if (ImGui::Shortcut(ImGuiKey_Backspace) || ImGui::Shortcut(ImGuiKey_Delete))
					{
						Cells[Row][Column] = 0;
						UpdateIncorrectCells();
					}
				}
			}
		}

		if (ImGui::IsKeyDown(ImGuiKey_Enter))
		{
			if (SelectedCell == -1)
			{
				SelectedCell = 0;
			}

			if (!PrefilledCells[SelectedCell])
			{
				SolveStep(SelectedCell);
			}

			SelectedCell = (SelectedCell + 1) % 81;
		}
	}

	if (ImGui::IsKeyReleased(ImGuiKey_Space))
	{
		printf("%s\n", Export());
	}
}

int Sudoku::CountFilledCells() const
{
	int Result = 0;

	IterateCells([this, &Result](const ImVec2& CellMin, const ImVec2& CellMax, int Row, int Column) -> bool
		{
			if (Cells[Row][Column] != 0)
			{
				Result++;
			}

			return true;
		});

	return Result;
}

int Sudoku::CountIncorrectCells() const
{
	int Result = 0;

	IterateCells([this, &Result](const ImVec2& CellMin, const ImVec2& CellMax, int Row, int Column) -> bool
		{
			int Cell = Helper::RowColumnToCell({ Row, Column });
			
			if (IncorrectCells[Cell])
			{
				Result++;
			}

			return true;
		});

	return Result;
}

const ImVec2 Sudoku::GetCellSize() const
{
	const ImVec2 Size = ImGui::GetWindowSize();

	const ImVec2 Center = Size / 2.f;
	const float SmallestAxis = min(Center.x, Center.y);

	const ImVec2 Radius = ImVec2(SmallestAxis, SmallestAxis) - PADDING;
	const ImVec2 CellSize = (Radius - PADDING) / 4.5f;

	return CellSize;
}

std::pair<ImVec2, ImVec2> Sudoku::GetCell(std::pair<int, int> RowColumn) const
{
	const auto [Row, Column] = RowColumn;

	const ImVec2 Size = ImGui::GetWindowSize();

	const ImVec2 Center = Size / 2.f;
	const ImVec2 CellSize = GetCellSize();

	float x = Center.x + (Row - 4.5f) * CellSize.x;
	float y = Center.y + (Column - 4.5f) * CellSize.y;

	ImVec2 Min = ImVec2(x, y);
	ImVec2 Max = Min + CellSize;

	return { Min, Max };
}

std::pair<ImVec2, ImVec2> Sudoku::GetCell(int Cell) const
{
	return GetCell(Helper::CellToRowColumn(Cell));
}

const ImU32 Sudoku::GetCellColor(int Cell, bool bSelected, bool bDirectlySelected, bool bIsSameValue) const
{
	bool bIsPrefilled = PrefilledCells[Cell];

	if (bSelected)
	{
		if (bDirectlySelected)
		{
			return bIsPrefilled ? PREFILLED_DIRECTLY_SELECTED_COL : DIRECTLY_SELECTED_COL;
		}
		else
		{
			if (bIsSameValue)
			{
				return bIsPrefilled ? PREFILLED_SAME_VALUE_INDIRECTLY_SELECTED_COL : SAME_VALUE_INDIRECTLY_SELECTED_COL;
			}
			else
			{
				return bIsPrefilled ? PREFILLED_INDIRECTLY_SELECTED_COL : INDIRECTLY_SELECTED_COL;
			}
		}
	}
	else
	{
		return bIsPrefilled ? PREFILLED_CELL_COL : DEFAULT_CELL_COL;
	}
}

const ImU32 Sudoku::GetCellColor(std::pair<int, int> RowColumn, bool bSelected, bool bDirectlySelected, bool bIsSameValue) const
{
	return GetCellColor(Helper::RowColumnToCell(RowColumn), bSelected, bDirectlySelected, bIsSameValue);
}

void Sudoku::DrawCandiates(int Cell) const
{
	const auto [Row, Column] = Helper::CellToRowColumn(Cell);

	// using Candidates[Row][Column][0 - 8]
}

void Sudoku::DrawCellValue(int CellValue, const ImVec2& CellMin, const ImVec2& CellMax) const
{
	if (CellValue == 0)
	{
		return;
	}

	const ImVec2 CellSize = CellMax - CellMin;
	const float CellHeight = CellSize.y;

	char Buf[2] = { '0' + CellValue, '\0'};

	const float OldFontScale = ImGui::GetIO().FontGlobalScale;
	const float FontScale = CellHeight / ImGui::GetFontSize();
	ImGui::SetWindowFontScale(FontScale);

	const ImVec2 TextSize = ImGui::CalcTextSize(Buf);
	const ImVec2 TextPos = CellMin + (CellSize - TextSize) / 2.f;

	ImGui::SetCursorScreenPos(TextPos);

	ImGui::PushStyleColor(ImGuiCol_Text, TEXT_COL);
	ImGui::Text(Buf);
	ImGui::PopStyleColor();

	ImGui::SetWindowFontScale(OldFontScale);
}

void Sudoku::DrawGrid() const
{
	const ImVec2 Size = ImGui::GetWindowSize();

	const ImVec2 Center = Size / 2.f;
	const float SmallestAxis = min(Center.x, Center.y);

	const ImVec2 Radius = ImVec2(SmallestAxis, SmallestAxis) - PADDING;
	const ImVec2 Min = Center - Radius;
	const ImVec2 Max = Center + Radius;

	ImDrawList* const DrawList = ImGui::GetWindowDrawList();

	DrawList->AddRect(Min, Max, PADDING_COL, 0.f, 0, PADDING_THICKNESS.x);

	IterateCells([DrawList, this](const ImVec2& CellMin, const ImVec2& CellMax, int Row, int Column) -> bool
		{
			DrawList->AddRect(CellMin, CellMax, GRID_COL, 0.f, 0, GRID_THICKNESS.x);

			return true;
		});

	IterateCells([DrawList](const ImVec2& CellMin, const ImVec2& CellMax, int Row, int Column) -> bool
		{
			if (Row % 3 == 0 && Column % 3 == 0)
			{
				const ImVec2 CellSize = CellMax - CellMin;

				DrawList->AddRect(CellMin, CellMin + 3 * CellSize, GRID_COL, 0.f, 0, BLOCK_THICKNESS.x);
			}

			return true;
		});
}

void Sudoku::DrawCells() const
{
	ImDrawList* const DrawList = ImGui::GetWindowDrawList();

	IterateCells([DrawList, this](const ImVec2& CellMin, const ImVec2& CellMax, int Row, int Column) -> bool
		{
			DrawList->AddRectFilled(CellMin, CellMax, GetCellColor({ Row, Column }, false, false, false));

			int CellValue = Cells[Row][Column];

			if (CellValue == 0)
			{
				int Cell = Helper::RowColumnToCell({ Row, Column });
				DrawCandiates(Cell);
			}

			DrawCellValue(CellValue, CellMin, CellMax);

			return true;
		});
}

void Sudoku::DrawSelectedCell() const
{
	if (SelectedCell == -1)
	{
		return;
	}
	
	const auto [Row, Column] = Helper::CellToRowColumn(SelectedCell);
	const auto [CellMin, CellMax] = GetCell({ Row, Column });
	const int SelectedCellValue = Cells[Row][Column];

	ImDrawList* const DrawList = ImGui::GetWindowDrawList();

	DrawList->AddRectFilled(CellMin, CellMax, GetCellColor(SelectedCell, true, true, false));
	DrawCellValue(SelectedCellValue, CellMin, CellMax);

	for (int CurrentRow = 0; CurrentRow < 9; CurrentRow++)
	{		
		const int CurrentCell = Helper::RowColumnToCell({ CurrentRow, Column });

		if (CurrentCell == SelectedCell)
		{
			continue;
		}

		const auto [IndirectlySelectedCellMin, IndirectlySelectedCellMax] = GetCell(CurrentCell);
		const int CurrentCellValue = Cells[CurrentRow][Column];

		DrawList->AddRectFilled(IndirectlySelectedCellMin, IndirectlySelectedCellMax, GetCellColor(CurrentCell, true, false, false));
		DrawCellValue(CurrentCellValue, IndirectlySelectedCellMin, IndirectlySelectedCellMax);
	}

	for (int CurrentColumn = 0; CurrentColumn < 9; CurrentColumn++)
	{
		const int CurrentCell = Helper::RowColumnToCell({ Row, CurrentColumn });

		if (CurrentCell == SelectedCell)
		{
			continue;
		}

		const auto [IndirectlySelectedCellMin, IndirectlySelectedCellMax] = GetCell(CurrentCell);
		const int CurrentCellValue = Cells[Row][CurrentColumn];

		DrawList->AddRectFilled(IndirectlySelectedCellMin, IndirectlySelectedCellMax, GetCellColor(CurrentCell, true, false, false));
		DrawCellValue(CurrentCellValue, IndirectlySelectedCellMin, IndirectlySelectedCellMax);
	}

	const int Block = Helper::CellToBlock(SelectedCell);
	const int StartingCell = Helper::BlockToCell(Block);

	for (int Offset : BlockOffsets)
	{
		const int CurrentCell = StartingCell + Offset;

		if (CurrentCell == SelectedCell)
		{
			continue;
		}

		const auto [IndirectlySelectedCellMin, IndirectlySelectedCellMax] = GetCell(CurrentCell);
		const auto [CurrentRow, CurrentColumn] = Helper::CellToRowColumn(CurrentCell);

		const int CurrentCellValue = Cells[CurrentRow][CurrentColumn];

		DrawList->AddRectFilled(IndirectlySelectedCellMin, IndirectlySelectedCellMax, GetCellColor(CurrentCell, true, false, false));
		DrawCellValue(CurrentCellValue, IndirectlySelectedCellMin, IndirectlySelectedCellMax);
	}

	if (SelectedCellValue != 0)
	{
		IterateCells([DrawList, SelectedCellValue, this](const ImVec2& CellMin, const ImVec2& CellMax, int Row, int Column) -> bool
			{
				const int CurrentCell = Helper::RowColumnToCell({ Row, Column });
				const int CurrentCellValue = Cells[Row][Column];

				if (CurrentCellValue != SelectedCellValue || CurrentCell == SelectedCell)
				{
					return true;
				}

				const auto [IndirectlySelectedCellMin, IndirectlySelectedCellMax] = GetCell(CurrentCell);

				DrawList->AddRectFilled(IndirectlySelectedCellMin, IndirectlySelectedCellMax, GetCellColor(CurrentCell, true, false, true));
				DrawCellValue(CurrentCellValue, IndirectlySelectedCellMin, IndirectlySelectedCellMax);

				return true;
			});
	}
}

void Sudoku::DrawIncorrectCells() const
{
	ImDrawList* const DrawList = ImGui::GetWindowDrawList();

	for (int Cell = 0; Cell < IM_ARRAYSIZE(IncorrectCells); Cell++)
	{
		if (!IncorrectCells[Cell]) // cell is not incorrect, skip
		{
			continue;
		}

		const auto [IncorrectCellMin, IncorrectCellMax] = GetCell(Cell);

		const ImVec2 CellSize = IncorrectCellMax - IncorrectCellMin;
		const ImVec2 DotPosition = IncorrectCellMin + CellSize * 0.8f;
		const  float DotRadius = CellSize.x / 12.5f;

		DrawList->AddCircleFilled(DotPosition, DotRadius, INCORRECT_DOT_COL);
	}
}

void Sudoku::Update(const float DeltaTime)
{
	HandleInput();

	if (IsSolved())
	{
		srand(time(nullptr));
		SolveMessageIdx = rand() % IM_ARRAYSIZE(SolveMessages);

		Import(SolveMessages[SolveMessageIdx]);

		bInputDisabled = true;
	}
}

void Sudoku::Draw(const float DeltaTime) const
{
	ImDrawList* const DrawList = ImGui::GetWindowDrawList();

	DrawList->AddRectFilled(ImVec2(0.f, 0.f), ImGui::GetWindowSize(), BG_COL);

	DrawCells();
	DrawSelectedCell();

	DrawGrid();
	DrawIncorrectCells();
}
