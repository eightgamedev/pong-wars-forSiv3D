# include <Siv3D.hpp>

enum class Team
{
	Red,
	Blue,
	Green,
	Yellow,
};

const HashTable<Team, Color> teamColors = {
	{ Team::Red, Palette::Red },
	{ Team::Blue, Palette::Blue },
	{ Team::Green, Palette::Green },
	{ Team::Yellow, Palette::Yellow },
};

class Ball
{
public:
	Ball(Color circleColor, Team team, const Vec2& pos)
		: m_circleColor(circleColor)
		, m_team(team)
		, m_pos(pos)
		, m_circle(400, 400, 8)
	{
		double velocity = Random(800);
		m_ballVelocity = { velocity, 800 - velocity };
	}

	void update()
	{
		m_pos.moveBy(m_ballVelocity * Scene::DeltaTime());
		m_circle.setPos(m_pos);
		bounceWall();
	}

	void draw() const
	{
		m_circle.draw(m_circleColor);
	}

	Circle getCircle() const
	{
		return m_circle;
	}

	Team getTeam() const
	{
		return m_team;
	}

	void bounce(RectF rect)
	{
		if (rect.right().intersects(m_circle)
					 || rect.left().intersects(m_circle))
		{
			bounceX();
		}

		if (rect.bottom().intersects(m_circle)
					 || rect.top().intersects(m_circle))
		{
			bounceY();
		}
	}

private:
	void bounceWall()
	{
		if (((m_pos.y < 0) && (m_ballVelocity.y < 0))
		|| ((Scene::Height() < m_pos.y) && (0 < m_ballVelocity.y)))
		{
			bounceY();
		}
		if (((m_pos.x < 0) && (m_ballVelocity.x < 0))
		|| ((Scene::Width() < m_pos.x) && (0 < m_ballVelocity.x)))
		{
			bounceX();
		}
	}

	void bounceX()
	{
		m_ballVelocity.x *= -1;
	}

	void bounceY()
	{
		m_ballVelocity.y *= -1;
	}

	Color m_circleColor;
	Team m_team;
	Vec2 m_pos;
	Circle m_circle;
	Vec2 m_ballVelocity;
};

class Brock
{
public:
	Brock() = default;
	Brock(const Vec2& pos, Team team, const Vec2& size)
		: m_team(team)
		, m_rect(pos, size)
		, m_color(teamColors.at(team))
	{
	}

	void draw() const
	{
		m_rect.stretched(-1).draw(m_color);
	}

	const RectF& getRect() const
	{
		return m_rect;
	}

	Team getTeam() const
	{
		return m_team;
	}

	void setTeam(Team team)
	{
		m_team = team;
		m_color = teamColors.at(team);
	}

private:
	RectF m_rect;
	Team m_team;
	Color m_color;
};;

void checkCollisionAndBounce(Ball& ball, Grid<Brock>& brocks) {
	for (auto& brock : brocks) {
		if (ball.getCircle().intersects(brock.getRect())) {
			if (ball.getTeam() == brock.getTeam())
			{
				continue;
			}

			ball.bounce(brock.getRect());
			brock.setTeam(ball.getTeam());
		}
	}
}

void Main()
{
	Window::Resize(800, 640);
	ResizeMode::Actual;

	Array<Ball> balls = {
		Ball{ Palette::Blue, Team::Red, Vec2{ 200, 200 } },
		Ball{ Palette::Red, Team::Blue, Vec2{ 200, 600 } },
		Ball{ Palette::Yellow, Team::Green,	Vec2{ 600, 200 } },
		Ball{ Palette::Green, Team::Yellow,	Vec2{ 600, 600 } },
	};

	constexpr Vec2 BrockSize(20, 20);
	const Point GridSize = Scene::Size() / BrockSize.asPoint();

	Grid<Brock> brocks(GridSize);

	for (int32 y = 0; y < GridSize.y; ++y)
	{
		for (int32 x = 0; x < GridSize.x; ++x)
		{
			Team team;
			if (x < GridSize.x / 2)
			{
				team = (y < GridSize.y / 2) ? Team::Red : Team::Blue;
			}
			else
			{
				team = (y < GridSize.y / 2) ? Team::Green : Team::Yellow;
			}
			brocks[y][x] = Brock{ Vec2(x, y) * BrockSize, team, BrockSize };
		}
	}

	while (System::Update())
	{
		for (auto& brock : brocks)
		{
			brock.draw();
		}

		for (auto& ball : balls)
		{
			ball.update();
			checkCollisionAndBounce(ball, brocks);
			ball.draw();
		}
	}
}
