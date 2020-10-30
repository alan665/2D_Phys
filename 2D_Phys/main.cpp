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
	aabb::rect vPlayer;

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
		vPlayer = { {100.0f, 10.0f}, {5.0f, 5.0f} };
		
		//Level rects
		vLevelRects.push_back({ {100.0f, 50.0f}, {20.0f, 20.0f} });
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

		// Clamp Player Velocity
		if (vPlayer.vel.x > maxVelocityX)
			vPlayer.vel.x = maxVelocityX;

		if (vPlayer.vel.x < -maxVelocityX)
			vPlayer.vel.x = -maxVelocityX;

		if (vPlayer.vel.y > maxVelocityY)
			vPlayer.vel.y = maxVelocityY;

		if (vPlayer.vel.y < -maxVelocityY)
			vPlayer.vel.y = -maxVelocityY;

		//Player Global Friction
		if (!isClimbing)
		{
			vPlayer.vel.x += -25.0f * vPlayer.vel.x * fElapsedTime;
			if (fabs(vPlayer.vel.x) < 0.02f)
				vPlayer.vel.x = 0.0f;
		}

		//Gravity
		vPlayer.vel.y += tmpGravity * fElapsedTime;

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
					vPlayer.vel.x += -moveSpeed;
				}
				if (GetKey(olc::Key::D).bHeld)
				{
					vPlayer.vel.x += +moveSpeed;
				}
				if (GetKey(olc::Key::SPACE).bPressed && vPlayer.contact[2])
				{
					vPlayer.contact[2] = nullptr;
					vPlayer.vel.y = sqrt(jumpHeight * tmpGravity) * -1;
				}
				if (GetKey(olc::Key::SHIFT).bHeld && (recentTouchedLeftWall || recentTouchedRightWall))
				{
					isClimbing = true;
					vPlayer.vel.x = 0;
					vPlayer.vel.y = 0;
				}
			}
		
		//Keyboard Input IsClimbing
			if (isClimbing)
			{
				if (GetKey(olc::Key::W).bHeld)
				{
					vPlayer.vel.y = -moveSpeed;
				}
				if (GetKey(olc::Key::W).bReleased)
				{
					vPlayer.vel.y = 0;
				}

				if (GetKey(olc::Key::S).bHeld)
				{
					vPlayer.vel.y = +moveSpeed;
				}
				if (GetKey(olc::Key::S).bReleased)
				{
					vPlayer.vel.y = 0;
				}

				if (GetKey(olc::Key::SPACE).bPressed && GetKey(olc::Key::A).bHeld)
				{
					vPlayer.vel.y += -jumpHeight * 4;
					vPlayer.vel.x += -jumpHeight * 8;
					Print("WALL JUMPED");
					isClimbing = false;
				}
				if (GetKey(olc::Key::SPACE).bPressed && GetKey(olc::Key::D).bHeld)
				{
					vPlayer.vel.y += -jumpHeight * 4;
					vPlayer.vel.x += +jumpHeight * 8;
					Print("WALL JUMPED");
					isClimbing = false;
				}
				if (GetKey(olc::Key::SHIFT).bReleased)
				{
					isClimbing = false;
				}
			}

		//Draw Player Collision Rect
		FillRect(vPlayer.pos, vPlayer.size, olc::WHITE);
		DrawRect(vPlayer.pos, vPlayer.size, olc::GREY);
		
		//Draw Level Rectangles
		for (const auto& r : vLevelRects)
			DrawRect(r.pos, r.size, olc::WHITE);

		//Sort collisions in order of distance
		olc::vf2d cp, cn;
		float t = 0, min_t = INFINITY;
		std::vector<std::pair<int, float>> z;
	
		//Work out collision point, add it to vector along with rect ID
		for (size_t i = 0; i < vLevelRects.size(); i++)
		{
			if (aabb::DynamicRectVsRect(&vPlayer, fElapsedTime, vLevelRects[i], cp, cn, t))
			{
				z.push_back({ i, t });
			}
		}

		//Do the sort
		std::sort(z.begin(), z.end(), [](const std::pair<int, float>& a, const std::pair<int, float>& b)
		{
			return a.second < b.second;
		});

		//Resolve the collision in order 
		for (auto j : z)
			aabb::ResolveDynamicRectVsRect(&vPlayer, fElapsedTime, &vLevelRects[j.first]);

		recentTouchedRightWall = false;
		recentTouchedLeftWall = false;

		//Draw rect dark red if wall on the right
		if (vPlayer.contact[1])
		{
			recentTouchedRightWall = true;
			DrawRect(vPlayer.contact[1]->pos, vPlayer.contact[1]->size, olc::DARK_RED);
			vPlayer.contact[1] = nullptr;
		}

		//Draw rect dark yellow if wall on the left
		if (vPlayer.contact[3])
		{
			recentTouchedLeftWall = true;
			DrawRect(vPlayer.contact[3]->pos, vPlayer.contact[3]->size, olc::DARK_YELLOW);
			vPlayer.contact[3] = nullptr;
		}

		// Add velocity to player
		vPlayer.pos += vPlayer.vel * fElapsedTime;

		//Draw players velocity vector (If this shown and the player is static then we have stored vel)
		if (vPlayer.vel.mag2() > 0)
			DrawLine(vPlayer.pos + vPlayer.size / 2, vPlayer.pos + vPlayer.size / 2 + vPlayer.vel.norm() * 10, olc::RED);

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