#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
// Override base class with your custom functionality


struct GameOfLife {
	GameOfLife(olc::PixelGameEngine* PGE = nullptr) {
		this->PGE = PGE;
		this->tiles.LoadFromFile("MarchingTiles8.png");
		this->width = ((PGE->ScreenWidth() - 64) / tiles.height);
		this->height = (PGE->ScreenHeight() / tiles.height);
		bMap.reserve(this->width * this->height);
		tempMap.reserve(this->width * this->height);
		iMap.reserve(this->width * this->height);
		for (int i = 0; i < width * height; i++) {
			bMap.push_back(rand() % 100 < 42);
			tempMap.push_back(0);
			iMap.push_back(0);
		}
	}

	bool Reset() {
		for (int i = 0; i < width * height; i++) bMap.at(i) = (rand() % 100 < 42);
		return true;
	}
	int Wrap(int i, int range) {
		while (i >= range) i -= range;
		while (i < 0) i += range;
		return i;
	}

	int GetIndex(int x, int y) {
		return y * width + x;
	}

	int Count(int x, int y) {
		int returnVal = 0;
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				if (i != 0 || j != 0) if (bMap.at(GetIndex(Wrap(x + i, width), Wrap(y + j, height)))) returnVal++;
			}
		}
		return returnVal;
	}

	int Value(int x, int y) {
		int returnVal = 0;
		if (bMap.at(GetIndex(Wrap(x, width), Wrap(y, height)))) returnVal += 1;
		if (bMap.at(GetIndex(Wrap(x+1, width), Wrap(y, height)))) returnVal += 2;
		if (bMap.at(GetIndex(Wrap(x, width), Wrap(y+1, height)))) returnVal += 4;
		if (bMap.at(GetIndex(Wrap(x+1, width), Wrap(y+1, height)))) returnVal += 8;
		
		return returnVal;
	}

	int Update(float fElapsedTime) {
		if(running)timer += fElapsedTime;
		if(btnTimer < 1) btnTimer += fElapsedTime;
		if (PGE->GetKey(olc::Key::R).bReleased) Reset();
		if (PGE->GetKey(olc::Key::P).bReleased) running = !running;
		if (PGE->GetKey(olc::Key::S).bReleased) timer = 10;
		if (btnTimer > 0.1) {
			if (PGE->GetKey(olc::Key::UP).bHeld) speed++;
			if (PGE->GetKey(olc::Key::DOWN).bHeld) if (speed > 1) speed--;
			btnTimer = 0;
		}
		if (PGE->GetMouse(0).bReleased) {
			int x = PGE->GetMouseX();
			int y = PGE->GetMouseY();
			if (x > PGE->ScreenWidth() - 36 && y > 42) {
				int i = (x - (PGE->ScreenWidth() - 36)) / 16;
				int j = (y - 42) / 16;
				if (j <= 8 && i < 2) {
					liveReact[i][j] = !liveReact[i][j];
				}
			}
		}
		if (timer > 1 / speed) {
			for (int i = 0; i < width * height; i++) {
				tempMap.at(i) = liveReact[bMap.at(i)][Count(i % width, i / width)];
			}
			for (int i = 0; i < width * height; i++) bMap.at(i) = tempMap.at(i);
			PGE->Clear(olc::VERY_DARK_BLUE);
			PGE->FillRect(PGE->ScreenWidth() - 60, 0, 60, PGE->ScreenHeight(), olc::VERY_DARK_GREEN);
			PGE->DrawString(PGE->ScreenWidth() - 36, 30, std::to_string((int)speed), olc::GREY);
			for (int i = 0; i < (width * height); i++) {
				iMap.at(i) = Value(i % width, i / width);
				PGE->DrawPartialSprite((i % width) * tiles.height, (i / width) * tiles.height, &tiles, iMap.at(i) * tiles.height, 0, tiles.height, tiles.height);
			}
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 9; j++) {
					PGE->FillRect(i * 16 + PGE->ScreenWidth() - 36, j * 16 + 42, 11, 11, olc::WHITE);
					if (liveReact[i][j]) PGE->FillCircle(i * 16 + PGE->ScreenWidth() - 31, j * 16 + 47, 4, olc::BLACK);
				}
			timer = 0;
		}
		if (!running) {
			PGE->FillRect(PGE->ScreenWidth() - 60, 0, 60, PGE->ScreenHeight(), olc::VERY_DARK_GREY);
			PGE->DrawString(PGE->ScreenWidth() - 36, 30, std::to_string((int)speed), olc::GREY);
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 9; j++) {
					PGE->FillRect(i * 16 + PGE->ScreenWidth() - 36, j * 16 + 42, 11, 11, olc::WHITE);
					if (liveReact[i][j]) PGE->FillCircle(i * 16 + PGE->ScreenWidth() - 31, j * 16 + 47, 4, olc::BLACK);
				}
		}
		
		return 0;
	}
	bool running = false;
	bool liveReact[2][9] = { {0,0,0,1,0,0,0,0,0}, {0,0,1,1,0,0,0,0,0} };
	std::vector<bool> bMap;
	std::vector<bool> tempMap;
	std::vector<int> iMap;
	olc::PixelGameEngine* PGE;
	olc::Sprite tiles;
	float timer = 10, speed = 1, btnTimer = 0;
	int width, height;
};





class Example : public olc::PixelGameEngine
{
public:
	


	Example()
	{
		sAppName = "GAME OF LIFE";
	}

	bool OnUserCreate() override
	{
		GOL = new GameOfLife(this);
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		
		GOL->Update(fElapsedTime);
		return true;
	}
	GameOfLife* GOL;
};
int main()
{
	Example demo;
	if (demo.Construct(480, 320, 2, 2))
		demo.Start();
	return 0;
}