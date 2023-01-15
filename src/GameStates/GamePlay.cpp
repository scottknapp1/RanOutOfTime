#include "GamePlay.h"
GamePlay::GamePlay(
  STATE state_id, sf::RenderWindow& game_window, sf::Font& game_font) :
  GameStateBase(state_id, game_window, game_font)
{
  player = new Player(window);

  // enemy array
  for (auto & i : enemy)
  {
    i = new Enemy(window);
  }

  for (auto & i : springs)
  {
    i = new Spring(window);
  }

  level = new Level(window);
}

GamePlay::~GamePlay()
{
  delete player;

  for(auto& i :enemy)
  {
    delete i;
  }

  for(auto& i : springs)
  {
    delete i;
  }

  delete level;
}

bool GamePlay::init()
{
  int x_pos_enemy[spring_count] = { 900, 1000, 2030, 2590 };
  int y_pos_enemy[spring_count] = { 3000, 2750, 2030, 2590 };


  if (!player->init())
  {
    return false;
  }

  if (!Level::loadAssets()) return false;

  for(auto & i : enemy)
  {
    if (!i->init(0,0))
    {
      return false;
    }
  }
  for (int i = 0; i < spring_count; ++i)
  {
    (springs[i]->initSpring(x_pos_enemy[i],y_pos_enemy[i]));
  }

  if (!level->init(player))
  {
    return false;
  }

  std::cout << "Gameplay init" << std::endl;

  return true;
}

void GamePlay::reset()
{
  player->reset();

  level->reset();

  transition = game_state_id;
}

void GamePlay::input(sf::Event event)
{
  level->input(event);

  if (event.type == sf::Event::KeyPressed)
  {
    player->keyPressed(event);
  }

  if (event.type == sf::Event::KeyReleased)
  {
    player->keyReleased(event);
  }
}

STATE GamePlay::update(float dt)
{
  checkSpringCollisions(dt);
  //reset spring
  for(auto & spring : springs)
  {
    if(!spring->isReady())
    {
      if(spring->getClock().getElapsedTime() >= sf::seconds(0.8f))
      {
        spring->reset();
      }
    }
  }

  sf::Vector2f player_pos(window.getSize().x / 2, player->getSprite()->getPosition().y);
  if (player_pos.y > level->getHeight() * 60 - window.getSize().y / 2)
  {
    player_pos.y = level->getHeight() * 60 - window.getSize().y / 2;
  }
  else if (player_pos.y + window.getSize().y / 2 < level->getHeight() * 60 - window.getSize().y / 2)
  {
    //player_pos.y = window.getSize().y / 2;
  }
  new_view.setCenter(player_pos);
  new_view.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));

  player->update(dt);

  for (auto & i : enemy)
  {
    i->update(dt);
  }

  for (auto & i : springs)
  {
    i->update(dt);
  }

  return level->update(dt);
}

void GamePlay::render()
{
  window.setView(new_view);
  for (auto & i : enemy)
  {
    i->render();
  }


  level->render();
  for (auto & i : springs)
  {
    i->render();
  }
}

void  GamePlay::checkSpringCollisions(float dt)
{
  for(auto & i : springs)
  {
    if(i->isColliding(*player->getSprite(),*i->getSprite()))
    {
      if(player->getSprite()->getPosition().y < i->getSprite()->getPosition().y)
      {
        if(i->isReady())
        {
          i->bounce(*player, dt);
        }
      }
    }
  }
}