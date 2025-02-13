#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

// global declaractions
// --------------------------------------------------------------------------------
Color green = {173, 204, 96, 255};
Color green_dark = {43, 51, 24, 255};

int cell_size = 30;
int cell_count = 25;
int offset = 75;

double time_last_update = 0;

bool CheckElementInDeque(Vector2 element, std::deque<Vector2> deque)
{
	for (unsigned int i = 0; i < deque.size(); i += 1)
	{
		if (Vector2Equals(deque[i], element))
		{
			return true;
		}
	}

	return false;
}
bool TriggerEvent(double interval)
{
	double time_current = GetTime();

	if (time_current - time_last_update >= interval)
	{
		time_last_update = time_current;

		return true;
	}

	return false;
}

class Food
{
public:
	Vector2 position;
	Texture2D texture;

	Vector2 GenerateRandomCell()
	{
		float x = GetRandomValue(0, cell_count - 1);
		float y = GetRandomValue(0, cell_count - 1);

		return Vector2{x, y};
	}
	Vector2 GenerateRandomPosition(std::deque<Vector2> snake_body)
	{

		Vector2 position_new = GenerateRandomCell();

		while (CheckElementInDeque(position_new, snake_body))
		{
			position_new = GenerateRandomCell();
		}

		return position_new;
	}
	void Draw()
	{
		DrawTexture(texture, offset + position.x * cell_size, offset + position.y * cell_size, WHITE);
	}

	// constructor / destructor
	Food(std::deque<Vector2> snake_body)
	{
		Image image = LoadImage("./graphics/food.png");
		texture = LoadTextureFromImage(image);
		UnloadImage(image);
		position = GenerateRandomPosition(snake_body);
	}
	~Food()
	{
		UnloadTexture(texture);
	}
};
class Snake
{
public:
	std::deque<Vector2> body = {
			Vector2{6, 9},
			Vector2{5, 9},
			Vector2{4, 9},
	};
	Vector2 direction{1, 0};
	bool is_segment_add = false;

	void Reset()
	{
		body = {
				Vector2{6, 9},
				Vector2{5, 9},
				Vector2{4, 9},
		};
		direction = {1, 0};
	}
	void Update()
	{
		body.push_front(Vector2Add(body[0], direction));

		if (is_segment_add)
		{
			is_segment_add = false;
		}
		else
		{
			body.pop_back();
		}
	}
	void Draw()
	{
		for (int i = 0; i < body.size(); i += 1)
		{
			float x = body[i].x;
			float y = body[i].y;

			Rectangle segment = Rectangle{offset + x * cell_size, offset + y * cell_size, float(cell_size), float(cell_size)};
			DrawRectangleRounded(segment, 0.5, 6, green_dark);
		}
	}
};
class Game
{
public:
	Snake snake = Snake();
	Food food = Food(snake.body);
	bool is_game_update_on = true;
	int score = 0;
	Sound sound_eat;
	Sound sound_wall;

	void CheckCollisionWithFood()
	{
		if (Vector2Equals(snake.body[0], food.position))
		{
			food.position = food.GenerateRandomPosition(snake.body);
			snake.is_segment_add = true;
			score += 1;
			PlaySound(sound_eat);
		}
	}
	void CheckCollisionWithEdges()
	{
		if (snake.body[0].y == -1 || snake.body[0].x == cell_count || snake.body[0].y == cell_count || snake.body[0].x == -1)
		{
			EndGame();
		}
	}
	void CheckCollisionWithTail()
	{
		std::deque<Vector2> headless_body = snake.body;
		headless_body.pop_front();

		if (CheckElementInDeque(snake.body[0], headless_body))
		{
			EndGame();
		}
	}
	void EndGame()
	{
		snake.Reset();
		food.position = food.GenerateRandomPosition(snake.body);
		is_game_update_on = false;
		score = 0;
		PlaySound(sound_wall);
	}
	void Update()
	{
		if (is_game_update_on)
		{
			snake.Update();
			CheckCollisionWithFood();
			CheckCollisionWithEdges();
			CheckCollisionWithTail();
		}
	}
	void Draw()
	{
		food.Draw();
		snake.Draw();
	}

	// constructor / destructor
	Game()
	{
		InitAudioDevice();
		sound_eat = LoadSound("./sounds/eat.mp3");
		sound_wall = LoadSound("./sounds/wall.mp3");
	}
	~Game()
	{
		UnloadSound(sound_eat);
		UnloadSound(sound_wall);
		CloseAudioDevice();
	}
};

// --------------------------------------------------------------------------------
// entry point
// --------------------------------------------------------------------------------
int main()
{
	// initialization
	// --------------------------------------------------------------------------------
	const int SCREEN_WIDTH = cell_size * cell_count;
	const int SCREEN_HEIGHT = cell_size * cell_count;

	InitWindow(2 * offset + SCREEN_WIDTH, 2 * offset + SCREEN_HEIGHT, "Retro Snake");
	SetTargetFPS(60);

	Game game = Game();

	// game loop
	// --------------------------------------------------------------------------------
	while (!WindowShouldClose())
	{
		// update objects
		// --------------------------------------------------------------------------------
		if (TriggerEvent(0.2))
		{
			game.Update();
		}
		if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
		{
			game.snake.direction = {0, -1};
			game.is_game_update_on = true;
		}
		if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
		{
			game.snake.direction = {1, 0};
			game.is_game_update_on = true;
		}
		if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
		{
			game.snake.direction = {0, 1};
			game.is_game_update_on = true;
		}
		if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
		{
			game.snake.direction = {-1, 0};
			game.is_game_update_on = true;
		}

		// draw objects
		// --------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(green);
		DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cell_size * cell_count + 10, (float)cell_size * cell_count + 10}, 5, green_dark);
		DrawText("Retro Snake", offset - 5, 20, 40, green_dark);
		DrawText(TextFormat("%i", game.score), offset - 5, offset + cell_size * cell_count + 10, 40, green_dark);
		game.Draw();

		EndDrawing();
	}

	// termination
	// --------------------------------------------------------------------------------
	CloseWindow();
	return 0;
}
