// Lab 8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

const float DUCK_SPEED = 0.2;

void LoadTex(Texture& tex, string filename)
{
    if (!tex.loadFromFile(filename))
    {
        cout << "Could not load " << filename << endl;
    }
}

void spawnDuck(World& world, PhysicsShapeList<PhysicsSprite>& ducks, Texture& redTex, PhysicsSprite& arrow, bool& drawingArrow, int& score, PhysicsRectangle& right)
{
    PhysicsSprite& duck = ducks.Create();
    duck.setTexture(redTex);

    Vector2f sz = duck.getSize();
    duck.setCenter(Vector2f(sz.x / 2.0, 20 + (sz.y / 2)));

    duck.setVelocity(Vector2f(DUCK_SPEED, 0));

    world.AddPhysicsBody(duck);

    duck.onCollision = [&drawingArrow, &world, &arrow, &duck, &ducks, &score, &right]
    (PhysicsBodyCollisionResult result)
        {
            if (result.object2 == arrow)
            {
                drawingArrow = false;
                world.RemovePhysicsBody(arrow);
                world.RemovePhysicsBody(duck);
                ducks.QueueRemove(duck);
                score += 10;
            }
            else if (result.object2 == right)
            {
                world.RemovePhysicsBody(duck);
                ducks.QueueRemove(duck);
            }
        };
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "Duck Hunter");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);

    PhysicsSprite& crossBow = *new PhysicsSprite();
    Texture cbowTex;
    LoadTex(cbowTex, "images/crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter(Vector2f(400, 600 - (sz.y / 2)));

    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "images/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    PhysicsRectangle left;
    left.setSize(Vector2f(10, 600));
    left.setCenter(Vector2f(5, 300));
    left.setStatic(true);
    world.AddPhysicsBody(left);

    PhysicsRectangle right;
    right.setSize(Vector2f(10, 600));
    right.setCenter(Vector2f(795, 300));
    right.setStatic(true);
    world.AddPhysicsBody(right);

    Texture redTex;
    LoadTex(redTex, "images/duck.png");
    PhysicsShapeList<PhysicsSprite> ducks;

    sf::Clock duckSpawnClock;
    float duckSpawnInterval = 1.0f;
    // spawn a new duck every second

    top.onCollision = [&drawingArrow, &world, &arrow]
    (PhysicsBodyCollisionResult result)
        {
            drawingArrow = false;
            world.RemovePhysicsBody(arrow);
        };

    Text scoreText;
    Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        cout << "Couldn't load font arial.ttf" << endl;
        exit(1);
    }
    scoreText.setFont(font);
    Text arrowCountText;
    arrowCountText.setFont(font);

    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);

    while ((arrows > 0) || drawingArrow)
    {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        if (deltaMS > 9)
        {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            if (Keyboard::isKeyPressed(Keyboard::Space) && !drawingArrow)
            {
                drawingArrow = true;
                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);
                arrows -= 1;
            }


            if (duckSpawnClock.getElapsedTime().asSeconds() >= duckSpawnInterval)
            {
                spawnDuck(world, ducks, redTex, arrow, drawingArrow, score, right);
                duckSpawnClock.restart();
            }

            window.clear();
            if (drawingArrow)
            {
                window.draw(arrow);
            }
            for (PhysicsShape& duck : ducks)
            {
                window.draw((PhysicsSprite&)duck);
            }
            window.draw(crossBow);
            scoreText.setString(to_string(score));
            FloatRect textBounds = scoreText.getGlobalBounds();
            scoreText.setPosition(
                Vector2f(780 - textBounds.width, 580 - textBounds.height));
            window.draw(scoreText);
            arrowCountText.setString(to_string(arrows));
            textBounds = arrowCountText.getGlobalBounds();
            arrowCountText.setPosition(
                Vector2f(20, 580 - textBounds.height));
            window.draw(arrowCountText);
            window.display();
            ducks.DoRemovals();
        }
    }
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    FloatRect textBounds = gameOverText.getGlobalBounds();
    gameOverText.setPosition(Vector2f(400 - (textBounds.width / 2), 300 - (textBounds.height / 2)));
    window.draw(gameOverText);
    window.display();
    while (true);
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
