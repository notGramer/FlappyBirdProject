#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System/String.hpp>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include "Sprites/background_png.h"
#include "Sprites/flappy_png.h"
#include "Sprites/exit_png.h"
#include "Sprites/game_over_png.h"
#include "Sprites/menu_png.h"
#include "Sprites/pipe_png.h"
#include "arial_ttf.h"
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
bool debugColl = true;
// Pillar
int PillarBoundPosX = 7;
int PillarBoundPosY = 8;
int PillarBoundSizeX = 15;
int PillarBoundSizeY= 10;
// Bird
int BirdBoundPosX = 40;
int BirdBoundPosY = 20;
int BirdBoundSizeX = 70;
int BirdBoundSizeY = 85;

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
        if (!texturePillar.loadFromMemory(Sprites_Pipe_png, Sprites_Pipe_png_len))
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

    void drawDebug(sf::RenderWindow &window)
    {
        for (Pillar* pillar : pillars)  // ← loop through each pillar
        {
            sf::FloatRect bounds = pillar->spritePillar->getGlobalBounds();  // ← get THIS pillar's bounds
            bounds = sf::FloatRect(
                {bounds.position.x + PillarBoundPosX, bounds.position.y + PillarBoundPosY},
                {bounds.size.x - PillarBoundSizeX, bounds.size.y - PillarBoundSizeY}
            );

            sf::RectangleShape box({bounds.size.x, bounds.size.y});
            box.setPosition(bounds.position);
            box.setFillColor(sf::Color::Transparent);
            box.setOutlineColor(sf::Color::Blue);
            box.setOutlineThickness(2.f);
            window.draw(box);
        }
    }
};

class Character
{
    public:
    sf::Texture texture;
    sf::Sprite* sprite;

    Character()
    {
    if (!texture.loadFromMemory(Sprites_flappy_png, Sprites_flappy_png_len))
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
        Bounds = sf::FloatRect({Bounds.position.x + BirdBoundPosX, Bounds.position.y + BirdBoundPosY},
                               {Bounds.size.x - BirdBoundSizeX, Bounds.size.y - BirdBoundSizeY});

        
        for(Pillar* pillar : pool.pillars)
        {
            sf::FloatRect pillarBounds = pillar->spritePillar->getGlobalBounds();
            pillarBounds = sf::FloatRect
            (
                {pillarBounds.position.x + PillarBoundPosX, pillarBounds.position.y + PillarBoundPosY},
                {pillarBounds.size.x - PillarBoundSizeX, pillarBounds.size.y - PillarBoundSizeY}
            );

            if(Bounds.findIntersection(pillarBounds) || Bounds.position.y <= 0|| Bounds.position.y > windowsHight)
            {
                isPaused = true;
            }
        }
    
    }

    void drawDebug(sf::RenderWindow &window, PillarPool &pool)
    {
        sf::FloatRect Bounds = sprite->getGlobalBounds();
        Bounds = sf::FloatRect({Bounds.position.x + BirdBoundPosX, Bounds.position.y + BirdBoundPosY},
                               {Bounds.size.x - BirdBoundSizeX, Bounds.size.y - BirdBoundSizeY});      
                               
        sf::RectangleShape birdBox({Bounds.size.x, Bounds.size.y});
        birdBox.setPosition(Bounds.position);
        birdBox.setFillColor({sf::Color::Transparent});
        birdBox.setOutlineColor({sf::Color::Blue});
        birdBox.setOutlineThickness(2.f);
        window.draw(birdBox);
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
        if (!textureButton.loadFromMemory(Sprites_menu_png, Sprites_menu_png_len))
        {
            cerr << "Failed to load texture" << endl;
            exit(1);
        }
        spriteButton = new sf::Sprite(textureButton);
        spriteButton->setPosition({950.0f, 0.0f});
        spriteButton->setScale({0.1f, 0.1f});
        Bounds = spriteButton->getGlobalBounds();
    }

    void GameOver()
    {

    if (!textureGameOver.loadFromMemory(Sprites_game_over_png, Sprites_game_over_png_len))
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


class Menu
{
    public:
    Button MenuButton;
    sf::RectangleShape *Box;
    bool MenuOpen = false;


    Menu()
    {
        Box = new sf::RectangleShape();        
        Box->setSize({500.f, 800.f});
        Box->setPosition({250.f,100.f});
        Box->setOutlineColor(sf::Color::Black);
        Box->setFillColor(sf::Color::White);
        Box->setOutlineThickness(2.f);
    }


    void draw(sf::RenderWindow &window)
    {
        if(isPaused)
        {
            window.draw(*Box);
        }
        
        
    }

    void update(sf::RenderWindow &window, Character &bird, PillarPool &pool)
    {
        window.draw(*MenuButton.spriteButton);


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) && !isPaused && !MenuOpen)
        {
            isPaused = true;
            MenuOpen = true;
            window.draw(*Box);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) && isPaused)
            isPaused = false;
            MenuOpen = false;
        }
};

void Exit()
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E))
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
    if (!background.loadFromMemory(Sprites_background_png, Sprites_background_png_len))
    {
        cerr << "Failed to load texture: Background.png" << std::endl;
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

    sf::Font font;
    if (!font.openFromMemory(ARIAL_TTF, ARIAL_TTF_len))
    {
        cerr << "Failed to load font" << endl;
        exit(1);
    }
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
        gameMenu.update(window, Bird, pillarPool);
        ScoreBoard.setString(num);
        Bird.collision(pillarPool);
        Bird.counter(pillarPool);


        // set !debugColl to turn debugging off
        // set debugColl to turn debbugin on
        // its just visual to know what where collides
        if (debugColl)
        {
        pillarPool.drawDebug(window);
        Bird.drawDebug(window, pillarPool);
        }

        gameMenu.MenuButton.onClick(window, Bird, pillarPool, spawnClock);
        window.display();
    }
}