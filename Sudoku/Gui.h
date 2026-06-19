#pragma once

#include "Includes.h"

#include "Sudoku.h"

#define DEFAULT_TITLE "Gui Window"
#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

class Gui
{
private:

	// Window properties
	const char*			Title;
	int					Width;
	int					Height;


	// GLFW properties
	GLFWwindow*			GLFWWindow;
	bool				bUseVSync;


	// ImGui properties
	ImGuiContext*		ImGuiContext;
	ImGuiViewport*		ImGuiViewport;

	
	// Sudoku properties
	std::unique_ptr<Sudoku> SudokuInstance;

public:

	Gui(const char* InTitle, int InWidth, int InHeight, bool InbUseVSync);
	Gui();
	~Gui();

	void StartGui();

private:

	void InitializeGui();
	void ShutdownGui();
	void RenderGui();

private:

	static void ErrorCallback(int error_code, const char* description);
};
