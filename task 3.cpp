#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <optional>

const int WIDTH = 800;
const int HEIGHT = 600;
const int SIZE = 20; // size of each grid block

enum Direction { UP, DOWN, LEFT, RIGHT };

struct SnakeSegment {
    int x, y;
    SnakeSegment(int x, int y) : x(x), y(y) {}
};

class Snake {
public:
    std::vector<SnakeSegment> body;
    Direction dir = RIGHT;

    Snake() {
        body.push_back(SnakeSegment(10, 10));
    }

    void move() {
        SnakeSegment head = body.front();
        switch (dir) {
            case UP: head.y -= 1; break;
            case DOWN: head.y += 1; break;
            case LEFT: head.x -= 1; break;
            case RIGHT: head.x += 1; break;
        }
        body.insert(body.begin(), head);
        body.pop_back();
    }

    void grow() {
        body.push_back(body.back());
    }

    bool checkCollision() {
        SnakeSegment head = body.front();
        for (size_t i = 1; i < body.size(); ++i) {
            if (head.x == body[i].x && head.y == body[i].y)
                return true;
        }
        return head.x < 0 || head.x * SIZE >= WIDTH || head.y < 0 || head.y * SIZE >= HEIGHT;
    }

    void setDirection(Direction newDir) {
        if ((dir == UP && newDir != DOWN) ||
            (dir == DOWN && newDir != UP) ||
            (dir == LEFT && newDir != RIGHT) ||
            (dir == RIGHT && newDir != LEFT)) {
            dir = newDir;
        }
    }
};

sf::Vector2i generateFoodPosition(const Snake& snake) {
    while (true) {
        int x = rand() % (WIDTH / SIZE);
        int y = rand() % (HEIGHT / SIZE);
        bool onSnake = false;
        for (auto segment : snake.body) {
            if (segment.x == x && segment.y == y) {
                onSnake = true;
                break;
            }
        }
        if (!onSnake) return {x, y};
    }
}

int main() {
    srand(time(nullptr));
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Snake Game");
    window.setFramerateLimit(60);

    Snake snake;
    sf::Vector2i foodPos = generateFoodPosition(snake);

    sf::RectangleShape snakeRect(sf::Vector2f(SIZE - 1, SIZE - 1));
    snakeRect.setFillColor(sf::Color::Green);
    sf::RectangleShape foodRect(sf::Vector2f(SIZE - 1, SIZE - 1));
    foodRect.setFillColor(sf::Color::Red);

    sf::SoundBuffer eatBuffer, gameOverBuffer;
    if (!eatBuffer.loadFromFile("assets/eat.wav")) {
    std::cerr << "Failed to load eat.wav!" << std::endl;
    return -1;
}

if (!gameOverBuffer.loadFromFile("assets/game_over.wav")) {
    std::cerr << "Failed to load game_over.wav!" << std::endl;
    return -1;
}

    sf::Sound eatSound(eatBuffer);
    sf::Sound gameOverSound(gameOverBuffer);

    sf::Font font;
    if (!font.openFromFile("assets/arial.ttf")) {
    std::cerr << "Failed to load font!" << std::endl;
    return -1;
}
    sf::Text scoreText(font, "Score: 0", 24);

    float timer = 0.0f, delay = 0.15f;
    int score = 0;

    sf::Clock clock;
    bool gameOver = false;

    while (window.isOpen()) {
        float time = clock.restart().asSeconds();
        timer += time;

       while (std::optional<sf::Event> event = window.pollEvent()) {
    if (event->is<sf::Event::Closed>()) {  // Correct event type check
        window.close();
    }

    if (auto keyEvent = event->getIf<sf::Event::KeyPressed>()) { // Use getIf instead
        switch (keyEvent->code) {
            case sf::Keyboard::Key::Up:    snake.setDirection(UP);    break;
            case sf::Keyboard::Key::Down:  snake.setDirection(DOWN);  break;
            case sf::Keyboard::Key::Left:  snake.setDirection(LEFT);  break;
            case sf::Keyboard::Key::Right: snake.setDirection(RIGHT); break;
            default: break;
        }
    }
}
        if (!gameOver && timer > delay) {
            timer = 0;
            snake.move();

            if (snake.checkCollision()) {
                gameOver = true;
                gameOverSound.play();
            }

            if (snake.body.front().x == foodPos.x && snake.body.front().y == foodPos.y) {
                snake.grow();
                foodPos = generateFoodPosition(snake);
                eatSound.play();
                score += 1;
                delay *= 0.97f;
            }
        }

        // Draw
        window.clear();
        for (auto& segment : snake.body) {
            snakeRect.setPosition(sf::Vector2f(segment.x * SIZE, segment.y * SIZE));
            window.draw(snakeRect);
        }

        foodRect.setPosition(sf::Vector2f(foodPos.x * SIZE, foodPos.y * SIZE));
        window.draw(foodRect);

        scoreText.setString("Score: " + std::to_string(score));
        window.draw(scoreText);

        if (gameOver) {
            sf::Text over(font, "GAME OVER", 48);
            over.setFillColor(sf::Color::Red);
            over.setPosition(sf::Vector2f(WIDTH / 2 - 150, HEIGHT / 2 - 50));
            window.draw(over);
        }

        window.display();
    }

    return 0;
}