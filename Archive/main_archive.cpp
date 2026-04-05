#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System/String.hpp>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <Sprites/background_png.h>
#include <Sprites/flappy_png.h>
#include <Sprites/exit_png.h>
#include <Sprites/game_over_png.h>
#include <Sprites/menu_png.h>
#include <Sprites/pipe_png.h>
#include <arial_ttf.h>
using namespace std;

const float zero = 0.0f;
const float gravity = 6.f;
const float jump_force = -17.f;
int windowsHight = 1000;
int windowWidth = 1000;
const float x = 250;
const float y = 250;
bool isPaused = false;
int i = 0;
string num = to_string(i);
bool restart = false;


class Pillar
{
    public:
    float pillarMovement = -3.f;
    sf::Texture texturePillar;
    sf::Sprite* spritePillar;
    bool counted = false;
    float rotation;

    
    Pillar(float rotation, float x, float y)
    {
        this->rotation = rotation;
        if (!texturePillar.loadFromFile("Sprites/Pipe.png"))
        {
            cerr << "Failed to load texture: flappy.png" << endl;
            exit(1);
        }
        spritePillar = new sf::Sprite(texturePillar);
        spritePillar->setPosition({x, y});
        spritePillar->setScale({0.3f, 0.3f});
        spritePillar->setRotation(sf::degrees(rotation));    
    }   

    void movementPillar()
    {
        spritePillar->move({pillarMovement, 0.0f});
    }

};

class PillarPool
{
    public:
    vector<Pillar*> pillars;

    void spawnPair()
    {
        float gapY = 300 + rand() % 400;
        float gapSize = -300.0f;
        Pillar* pillarUp = new Pillar(0, 1000, gapY - gapSize);
        Pillar* pillarDown = new Pillar(180, 1070, gapY);
        pillars.push_back(pillarDown);
        pillars.push_back(pillarUp);
    
    }

    void update()
    {
        for (Pillar* pillar : pillars)
        {
            pillar->movementPillar();
        }

        pillars.erase(remove_if(pillars.begin(), pillars.end(), [](Pillar* p){
            return p->spritePillar->getPosition().x < -70;
        }), pillars.end());
            }

    void draw(sf::RenderWindow &window)
    {
        for (Pillar* pillar : pillars)
        {
            window.draw(*pillar->spritePillar);
        }
    }

    void reset()
    {
        pillars.clear();
    }

};

class Character
{
    public:
    sf::Texture texture;
    sf::Sprite* sprite;

    Character()
    {
        if (!texture.loadFromFile/flappy.png"))
        {
            cerr << "Failed to load texture: flappy.png" << std::endl;
            exit(1);
        }
        sprite = new sf::Sprite(texture);
        sprite->setScale({.3f, .3f});
        sprite->setPosition({50.0f,50.0f});
        }

        void movementBird()
        {
            sprite->move({zero,gravity});

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
            {
                sprite->move({zero, jump_force});
            }
        }
    
    void collision(PillarPool &pool)
    {
        sf::FloatRect Bounds = sprite->getGlobalBounds();
        Bounds = sf::FloatRect({Bounds.position.x + 10, Bounds.position.y + 10},
                               {Bounds.size.x - 20, Bounds.size.y -20});

        
        for(Pillar* pillar : pool.pillars)
        {
            sf::FloatRect pillarBounds = pillar->spritePillar->getGlobalBounds();
            pillarBounds = sf::FloatRect
            (
                {pillarBounds.position.x + 20, pillarBounds.position.y + 20},
                {pillarBounds.size.x - 40, pillarBounds.size.y -40}
            );

            if(Bounds.findIntersection(pillarBounds) || Bounds.position.y <= 0|| Bounds.position.y > windowsHight)
            {
                isPaused = true;
            }
        }
    }

    void counter(PillarPool &pool)
    {
        sf::FloatRect Bounds = sprite->getGlobalBounds();


        for(Pillar* pillar : pool.pillars)
        {
            if(sprite->getPosition().x > pillar->spritePillar->getPosition().x && !pillar->counted && pillar->rotation == 0)
            {
                pillar->counted = true;
                i++;
                num = to_string(i);
            }
        }
    }
};

class Button
{
    public:
    sf::Texture textureButton;    
    sf::Sprite* spriteButton; 
    sf::FloatRect Bounds;
    sf::Texture textureGameOver;
    sf::Sprite* spriteGameOver;


    Button()
    {
        if (!textureButton.loadFromFile("Sprites/menu.png"))
        {
            cerr << "Failed to load texture" << endl;
            exit(1);
        }
        spriteButton = new sf::Sprite(textureButton);
        spriteButton->setPosition({950.0f, 0.0f});
        spriteButton->setScale({0.1f, 0.1f});
        Bounds = spriteButton->getGlobalBounds();
    }

    void restart()
    {

    if (!textureGameOver.loadFromFile("Sprites/game_over.png"))
    {
    cerr << "Failed to load texture" << endl;
        exit(1);
    }
    spriteGameOver = new sf::Sprite(textureGameOver);
    spriteGameOver->setPosition({500.0f, 500.0f});
    spriteGameOver->setScale({0.1f, 0.1f});

    }

    void onClick(sf::RenderWindow &window, Character &bird, PillarPool &pool, sf::Clock &clock)
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if(Bounds.contains(mousePos) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
              bird.sprite->setPosition({50.f, 50.f});
              pool.reset();
              clock.restart();
              i=0;
              num = to_string(i);
              isPaused = false;
        }
    }   
};

class MenuBox
{
    public:
    float x = 200.f, y = 200.f;
    sf::RectangleShape Box;

    sf::RectangleShape MenuBackground()
    {
        Box.setSize({500.f, 500.f});
        Box.setPosition({x,y});
        Box.setOutlineColor(sf::Color(0,0,0));
        Box.setFillColor(sf::Color(255, 255, 255));
        Box.setOutlineThickness(2.f);
        return Box;
    }

};

class Menu
{
    public:
    Button MenuButton;
    MenuBox MenuBackground;

    Menu()
    {

    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(MenuBackground.Box);
        window.draw(*MenuButton.spriteButton);
    }

    void update(Character &bird, PillarPool &pool)
    {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::M) && !isPaused)
        isPaused = true;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C) && isPaused)
        isPaused = false;
    }
};

void Exit()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
    {
        exit(0);
    }
}


int main()
{
    sf::RenderWindow window(sf::VideoMode({1000, 1000}), "FlappyBird!");
    window.setFramerateLimit(60);

    Character Bird;
    Menu gameMenu;
    PillarPool pillarPool;
    sf::Clock spawnClock;

    sf::Texture background;
    if (!background.loadFromFile("Sprites/background.png"))
    {
        cerr << "Failed to load texture: flappy.png" << std::endl;
        exit(1);
    }
    sf::Sprite spriteBackground(background);
    sf::Vector2u texSize = background.getSize();
    sf::Vector2u winSize = window.getSize();
    spriteBackground.setScale(
        {
            static_cast<float>(windowWidth) / background.getSize().x,
            static_cast<float>(windowsHight) / background.getSize().y
        }
    );
    spriteBackground.setPosition({0,0});

    const sf::Font font("ARIAL.TTF");
    sf::Text ScoreBoard(font);
    ScoreBoard.setString(num);
    ScoreBoard.setCharacterSize(40);
    ScoreBoard.setFillColor(sf::Color::Yellow);
    ScoreBoard.setStyle(sf::Text::Bold);
    ScoreBoard.setPosition({900,50});
    sf::RectangleShape backpanelScoreboard({100.0f, 50.0f});
    backpanelScoreboard.setFillColor(sf::Color::Black);
    backpanelScoreboard.setPosition({900,  50});

    vector<sf::Drawable*> drawings = {&spriteBackground, 
        &backpanelScoreboard, 
        &ScoreBoard,
        Bird.sprite 
        };


    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        Exit();

        window.clear();
        for (auto& drawable : drawings) {
            window.draw(*drawable);
        }
        if (spawnClock.getElapsedTime().asSeconds() > 3.0f)
        {
            pillarPool.spawnPair();
            spawnClock.restart();
        }

        if(!isPaused)
        {
            pillarPool.update();
            Bird.movementBird();
        }

        pillarPool.draw(window);
        gameMenu.draw(window);
        gameMenu.update(Bird, pillarPool);
        ScoreBoard.setString(num);
        Bird.collision(pillarPool);
        Bird.counter(pillarPool);
        gameMenu.MenuButton.onClick(window, Bird, pillarPool, spawnClock);
        window.display();
    }
}