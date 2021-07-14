#pragma once


class GameOption
{
public:
	int windows_mode;

	void ImGui();

	static GameOption& GetInstance()
	{
		static GameOption ins;
		return ins;
	}

private:
	GameOption();
	~GameOption();
};

#define GAME_OPTION GameOption::GetInstance()