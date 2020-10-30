#define OLC_PGE_APPLICATION
#include "2DPhys.h"

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

	bool recentTouchedRightWall = false;
	bool recentTouchedLeftWall = false;

	void Print(const std::string p)
	{
		std::cout << p << std::endl;
	}

	bool OnUserCreate() override
	{
		//Player Collision Rect
		vPlayer.push_back({ {100.0f, 10.0f}, {5.0f, 5.0f} }); //Player
		vPlayer.push_back({ {100.0f, 10.0f}, {5.0f, 5.0f} }); //PlayerLeftBoxDetector
		
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
		vLevelRects.push_back({ {6.5f, 50.0f}, {5.0f, 300.0f} });
		vLevelRects.push_back({ {300.0f, 50.0f}, {5.0f, 300.0f} });
		vLevelRects.push_back({ {260.0f, 50.0f}, {5.0f, 300.0f} });

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{	
		//Init
		Clear(olc::DARK_CYAN);

		//Player Phys Boxes Follow Player
		vPlayer[1].pos.x = vPlayer[0].pos.x + -6.5f;
		vPlayer[1].pos.y = vPlayer[0].pos.y;

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

		//Gravity
		vPlayer[0].vel.y += tmpGravity * fElapsedTime;

		//Disable gravity if climbing
		if (isClimbing)
			tmpGravity = 0;
		if (!isClimbing)
			tmpGravity = gravity;

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
				if (GetKey(olc::Key::SHIFT).bHeld && (recentTouchedLeftWall || recentTouchedRightWall))
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
				}
			}

		//Draw Player
		FillRect(vPlayer[0].pos, vPlayer[0].size, olc::WHITE);

		//Draw Player Left Wall Collision Rect
		DrawRect(vPlayer[1].pos, vPlayer[1].size, olc::GREY);
		
		//Draw Level Rectangles
		for (const auto& r : vLevelRects)
			DrawRect(r.pos, r.size, olc::WHITE);



		// === Player Rect Collisions == //


		//Sort collisions in order of distance
		olc::vf2d playerContactPoint, playerContactNormal;
		olc::vf2d leftContactPoint, leftContactNormal;
		float playerTime = 0, pmin_t = INFINITY;
		float leftTime = 0, tmin_t = INFINITY;
		std::vector<std::pair<int, float>> playerSort;
		std::vector<std::pair<int, float>> leftSort;

		//Work out collision point, add it to vector along with rect ID
		for (size_t i = 0; i < vLevelRects.size(); i++)
		{
			if (aabb::DynamicRectVsRect(&vPlayer[0], fElapsedTime, vLevelRects[i], playerContactPoint, playerContactNormal, playerTime))
			{
				playerSort.push_back({ i, playerTime });
			}
		}

		for (size_t it = 0; it < vLevelRects.size(); it++)
		{
			if (aabb::DynamicRectVsRect(&vPlayer[1], fElapsedTime, vLevelRects[it], leftContactPoint, leftContactNormal, leftTime))
			{
				leftSort.push_back({ it, leftTime });
			}
		}

		//Sort Player
		std::sort(playerSort.begin(), playerSort.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
		{
			return a.second < b.second;
		});

		//Sort Left
		std::sort(leftSort.begin(), leftSort.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
		{
			return a.second < b.second;
		});

		//Resolve player collision
		for (auto j : playerSort)
			aabb::ResolveDynamicRectVsRect(&vPlayer[0], fElapsedTime, &vLevelRects[j.first]);

		//Resolve left collision
		for (auto j : leftSort)
			aabb::ResolveDynamicRectVsRect(&vPlayer[1], fElapsedTime, &vLevelRects[j.first]);
	





		recentTouchedRightWall = false;
		recentTouchedLeftWall = false;

		//Attempt to find top of Rect player is climbing (Currently Left)
		if (vPlayer[1].contact[3])
		{
			Print("Colliding with top of rect and Player Should stop moving");
			vPlayer[1].contact[3] = nullptr;
		}

		//Draw rect dark red if wall on the right
		if (vPlayer[0].contact[1])
		{
			recentTouchedRightWall = true;
			DrawRect(vPlayer[0].contact[1]->pos, vPlayer[0].contact[1]->size, olc::DARK_RED);
			vPlayer[0].contact[1] = nullptr;
		}

		//Draw rect dark yellow if wall on the left
		if (vPlayer[0].contact[3])
		{
			recentTouchedLeftWall = true;
			DrawRect(vPlayer[0].contact[3]->pos, vPlayer[0].contact[3]->size, olc::DARK_YELLOW);
			vPlayer[0].contact[3] = nullptr;
		}

		// Add velocity to player
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
// All in contact rectangles in yellow
for (int i = 0; i < 4; i++)
{
	if (vRects[0].contact[i])
		DrawRect(vRects[0].contact[i]->pos, vRects[0].contact[i]->size, olc::YELLOW);
	vRects[0].contact[i] = nullptr;
}
*/