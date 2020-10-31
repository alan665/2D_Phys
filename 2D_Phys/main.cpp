#define OLC_PGE_APPLICATION
#include "2DPhys.h"
#include <iostream>
#include <cmath>

class Phys2D : public olc::PixelGameEngine
{
public:
	Phys2D()
	{
		sAppName = "Phys2D";
	}

	std::vector<aabb::rect> vLevelRects;
	std::vector<aabb::rect> vPlayer;

	float gravity = 1800.0f;
	float tmpGravity = gravity;
	float maxVelocityX = 40.0f;
	float maxVelocityY = 500.0f;
	float moveSpeed = 105.0f;
	float jumpHeight = 32 * 3.0f;

	bool isClimbing = false;
	bool isInAir = false;

	//Status stored until player stops climbing
	bool recentTouchedRightWall = false;
	bool recentTouchedLeftWall = false;

	//Status stored per tick
	bool tickTouchedRightWall = false;
	bool tickTouchedLeftWall = false;
	bool leftWallPresent = false;
	bool rightWallPresent = false;

	void Print(const std::string p)
	{
		std::cout << p << std::endl;
	}

	bool OnUserCreate() override
	{	
		//Player Collision Rect
		vPlayer.push_back({ {100.0f, 10.0f}, {5.0f, 5.0f} }); //Player

		vPlayer.push_back({ {80.0f, 5.0f}, {2.0f, 2.0f} }); //PlayerLeftBoxDetector
		vPlayer.push_back({ {80.0f, 5.0f}, {2.0f, 2.0f} }); //PlayerLeftBoxDetector
		
		//Level rects
		vLevelRects.push_back({ {100.0f, 40.0f}, {20.0f, 20.0f} }); //[0]
		vLevelRects.push_back({ {100.0f, 150.0f}, {75.0f, 20.0f} });
		vLevelRects.push_back({ {140.0f, 50.0f}, {20.0f, 20.0f} });
		vLevelRects.push_back({ {190.0f, 50.0f}, {20.0f, 20.0f} });
		vLevelRects.push_back({ {110.0f, 50.0f}, {20.0f, 20.0f} });
		vLevelRects.push_back({ {50.0f, 130.0f}, {20.0f, 20.0f} });
		vLevelRects.push_back({ {50.0f, 150.0f}, {20.0f, 20.0f} });
		vLevelRects.push_back({ {50.0f, 170.0f}, {20.0f, 20.0f} });
		vLevelRects.push_back({ {150.0f, 100.0f}, {10.0f, 1.0f} });
		vLevelRects.push_back({ {200.0f, 100.0f}, {20.0f, 60.0f} });
		vLevelRects.push_back({ {0.0f, 160.0f}, {500.0f, 10.0f} });
		vLevelRects.push_back({ {6.5f, 10.0f}, {5.0f, 600.0f} });
		vLevelRects.push_back({ {300.0f, 50.0f}, {5.0f, 300.0f} });

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{	
		//Init
		Clear(olc::DARK_CYAN);

		//Check if the player is at the top of the rect they are climbing, and move them to the top
		if (!rightWallPresent && !leftWallPresent && isClimbing)
		{
			if (recentTouchedRightWall)
			{
				vPlayer[0].vel.x += +jumpHeight * 8;
				isClimbing = false;
				recentTouchedRightWall = false;
				Print("Is at top of RIGHT Rect");
			}
			if (recentTouchedLeftWall)
			{
				vPlayer[0].vel.x += -jumpHeight * 8;
				isClimbing = false;
				recentTouchedLeftWall = false;
				Print("Is at top of LEFT Rect");
			}
		}

		//Player Left Phys Box Follow Player
		vPlayer[1].pos.x = vPlayer[0].pos.x + -3.0f;
		vPlayer[1].pos.y = vPlayer[0].pos.y + 6.0f;

		//Player Right Phys Box Follow Player
		vPlayer[2].pos.x = vPlayer[0].pos.x + 5.0f;
		vPlayer[2].pos.y = vPlayer[0].pos.y - -6.0f;

		// Clamp Player Velocity
		if (vPlayer[0].vel.x > maxVelocityX)
			vPlayer[0].vel.x = maxVelocityX;
		if (vPlayer[0].vel.x < -maxVelocityX)
			vPlayer[0].vel.x = -maxVelocityX;
		if (vPlayer[0].vel.y > maxVelocityY)
			vPlayer[0].vel.y = maxVelocityY;
		if (vPlayer[0].vel.y < -maxVelocityY)
			vPlayer[0].vel.y = -maxVelocityY;

		//Player Global Friction
		if (!isClimbing)
		{
			vPlayer[0].vel.x += -25.0f * vPlayer[0].vel.x * fElapsedTime;
			if (fabs(vPlayer[0].vel.x) < 0.02f)
				vPlayer[0].vel.x = 0.0f;
		}

		//Disable gravity if climbing
		if (isClimbing)
			tmpGravity = 0;
		else
			tmpGravity = gravity;

		//Player Gravity
		vPlayer[0].vel.y += tmpGravity * fElapsedTime;

		//Keyboard Input Walking
			if (!isClimbing)
			{
				if (GetKey(olc::Key::A).bHeld)
				{
					vPlayer[0].vel.x += -moveSpeed;
				}
				if (GetKey(olc::Key::D).bHeld)
				{
					vPlayer[0].vel.x += +moveSpeed;
				}
				if (GetKey(olc::Key::SPACE).bPressed && vPlayer[0].contact[2])
				{
					vPlayer[0].contact[2] = nullptr;
					vPlayer[0].vel.y = sqrt(jumpHeight * tmpGravity) * -1;
				}
				if (GetKey(olc::Key::SHIFT).bHeld && (tickTouchedLeftWall || tickTouchedRightWall))
				{
					isClimbing = true;
					vPlayer[0].vel.x = 0;
					vPlayer[0].vel.y = 0;
				}
			}
		
		//Keyboard Input IsClimbing
			if (isClimbing)
			{
				if (GetKey(olc::Key::W).bHeld)
				{
					vPlayer[0].vel.y = -moveSpeed;
				}
				if (GetKey(olc::Key::W).bReleased)
				{
					vPlayer[0].vel.y = 0;
				}

				if (GetKey(olc::Key::S).bHeld)
				{
					vPlayer[0].vel.y = +moveSpeed;
				}
				if (GetKey(olc::Key::S).bReleased)
				{
					vPlayer[0].vel.y = 0;
				}

				if (GetKey(olc::Key::SPACE).bPressed && GetKey(olc::Key::A).bHeld)
				{
					vPlayer[0].vel.y += -jumpHeight * 4;
					vPlayer[0].vel.x += -jumpHeight * 8;
					isClimbing = false;
				}
				if (GetKey(olc::Key::SPACE).bPressed && GetKey(olc::Key::D).bHeld)
				{
					vPlayer[0].vel.y += -jumpHeight * 4;
					vPlayer[0].vel.x += +jumpHeight * 8;
					isClimbing = false;
				}
				if (GetKey(olc::Key::SHIFT).bReleased)
				{
					isClimbing = false;
					recentTouchedRightWall = false;
					recentTouchedLeftWall = false;
				}
			}

		//Draw Player
		FillRect(vPlayer[0].pos, vPlayer[0].size, olc::WHITE);

		//Draw Player Collision Rects
		DrawRect(vPlayer[1].pos, vPlayer[1].size, olc::GREY);
		DrawRect(vPlayer[2].pos, vPlayer[2].size, olc::GREY);
		
		//Draw Level Rectangles
		for (const auto& r : vLevelRects)
		{
			DrawRect(r.pos, r.size, olc::WHITE);
		}

		//Sort collisions in order of distance
		olc::vf2d playerContactPoint, playerContactNormal;
		olc::vf2d leftContactPoint, leftContactNormal;
		olc::vf2d direction = { float(GetMouseX()), float(GetMouseY()) };
		float playerTime = 0, pmin_t = INFINITY;
		float leftTime = 0, lmin_t = INFINITY;
		std::vector<std::pair<int, float>> playerSort;

		//Reset wall status per tick
		rightWallPresent = false;
		leftWallPresent = false;

		//Work out collision point, add it to vector along with rect ID
		for (size_t i = 0; i < vLevelRects.size(); i++)
		{
			//Detect Player
			if (aabb::DynamicRectVsRect(&vPlayer[0], fElapsedTime, vLevelRects[i], playerContactPoint, playerContactNormal, playerTime))
			{
				playerSort.push_back({ i, playerTime });
			}

			if (aabb::PointVsRect(vPlayer[1].pos, &vLevelRects[i]))
			{
				leftWallPresent = true;
			}

			if (aabb::PointVsRect(vPlayer[2].pos, &vLevelRects[i]))
			{
				rightWallPresent = true;
			}
		}

		//Sort Player Collisions
		std::sort(playerSort.begin(), playerSort.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
		{
			return a.second < b.second;
		});

		//Resolve player collision
		for (auto i : playerSort)
		{
			aabb::ResolveDynamicRectVsRect(&vPlayer[0], fElapsedTime, &vLevelRects[i.first]);
		}

		//Reset wall touched Status for this tick
		tickTouchedRightWall = false;
		tickTouchedLeftWall = false;

		//Draw rect dark red if wall on the right
		if (vPlayer[0].contact[1])
		{
			tickTouchedRightWall = true;
			recentTouchedRightWall = true;
			DrawRect(vPlayer[0].contact[1]->pos, vPlayer[0].contact[1]->size, olc::DARK_RED);
			vPlayer[0].contact[1] = nullptr;
		}

		//Draw rect dark yellow if wall on the left
		if (vPlayer[0].contact[3])
		{
			tickTouchedLeftWall = true;
			recentTouchedLeftWall = true;
			DrawRect(vPlayer[0].contact[3]->pos, vPlayer[0].contact[3]->size, olc::DARK_YELLOW);
			vPlayer[0].contact[3] = nullptr;
		}

		//Add velocity to player
		vPlayer[0].pos += vPlayer[0].vel * fElapsedTime;

		//Draw players velocity vector (If this shown and the player is static then we have stored vel)
		if (vPlayer[0].vel.mag2() > 0)
			DrawLine(vPlayer[0].pos + vPlayer[0].size / 2, vPlayer[0].pos + vPlayer[0].size / 2 + vPlayer[0].vel.norm() * 10, olc::RED);

		return true;
	}
};

int main(){
	Phys2D demo;
	if (demo.Construct(320, 180, 4, 4, false, false))
		demo.Start();
	return 0;
}






/*
//In contact rectangles in yellow
for (int i = 0; i < 3; i++)
{
	if (vRects[0].contact[i])
		DrawRect(vRects[0].contact[i]->pos, vRects[0].contact[i]->size, olc::YELLOW);
	vRects[0].contact[i] = nullptr;
}
*/