#pragma warning(disable : 4996)
#include <SFML/Graphics.hpp>
#include "map.h" 
#include "menu.h"
#include "gameMenu.h"
#include "restartMenu.h"
#include <iostream>
#include <sstream>
#include <list>
#include <time.h>


using namespace sf;
using namespace std;

//базовый класс
class gameObj
{
public:
	float dx, dy, x, y, speed, moveTimer;
	int w, h, health;
	bool dead;
	Texture texture;
	Sprite sprite;
	String name;//вариант противника по имени
	virtual void update(float time) = 0;
	bool isShoot;
	gameObj(Image& image, String Name, float X, float Y, int W, int H)
	{
		x = X; y = Y; w = W; h = H; name = Name; moveTimer = 0;
		speed = 0; dx = 0; dy = 0; health = 100;
		dead = false;
		texture.loadFromImage(image);
		sprite.setTexture(texture);
	}
	FloatRect getRect()
	{
		return FloatRect(x, y, w, h);
	}
	float getObjXc()//c - Coordinate
	{
		return x;
	}
	float getObjYc()//c - Coordinate
	{
		return y;
	}
};
//-------------------------------------------класс игрока---------------------------------------
class Player :public gameObj
{
	float x, y;
public:
	enum { left, right, up, down } state;//управление
	int playerScore;
	//конструктор
	Player(Image& image, String Name,float X, float Y, int W, int H) : gameObj(image, Name, X, Y, W, H)
	{
		playerScore = 0;
		if (name == "Player")
		{
			sprite.setTextureRect(IntRect(0, 0, w, h));//IntRect-приведение типов, выводим один прямоугольник с кораблем	
		}
	}

	void control()
	{
		if ((Keyboard::isKeyPressed(Keyboard::Left) || (Keyboard::isKeyPressed(Keyboard::A))))
		{
			state = left;//направление влево. 	
			speed = 0.1;
		}
		if ((Keyboard::isKeyPressed(Keyboard::Right) || (Keyboard::isKeyPressed(Keyboard::D))))
		{
			state = right;//направление вправо		
			speed = 0.1;
		}
		if ((Keyboard::isKeyPressed(Keyboard::Up) || (Keyboard::isKeyPressed(Keyboard::W))))
		{
			state = up;//направление вверх		
			speed = 0.1;
		}
		if ((Keyboard::isKeyPressed(Keyboard::Down) || (Keyboard::isKeyPressed(Keyboard::S))))
		{
			state = down;//направление вниз
			speed = 0.1;
		}
	}
	void update(float time)
	{
		control();
		//убирает скорость по Z кординате (по диагонали)
		switch (state)
		{
		case right: dx = speed; dy = 0; break;
		case left: dx = -speed; dy = 0; break;
		case up: dy = -speed; dx = 0; break;
		case down: dy = speed; dx = 0; break;
		}
		//движение игрока
		x += dx * (time * 2.7);// 2.7 - ускорение для времени 
		if (x < 2) x = 2;
		else if (x > 970) x = 950;
		y += dy * (time * 2.7);
		if (y < 500) y = 500;
		else if (y > 790) y = 790;
		this->speed = 0;
		sprite.setPosition(x + w / 2, y + h / 2);
		sprite.setPosition(x, y);//вывод координат посередине
		if (this->health <= 0)
		{
			this->dead = true;
			this->speed = 0;
		}
		
	}
	float getPlayerXc()//c - Coordinate
	{
		return x;
	}
	float getPlayerYc()//c - Coordinate
	{
		return y;
	}
	
};
//-------------------------------------------класс врага----------------------------------------------
class Enemy : public gameObj
{
public:
	Enemy(Image& image, String Name, float X, float Y, int W, int H) : gameObj(image, Name, X, Y, W, H )
	{
		if (name == "EasyEnemy")
		{
			sprite.setTextureRect(IntRect(20, 0, 1209, 889));
			dx = 0.1;
		}
		if (name == "MediumEnemy")
		{
			sprite.setTextureRect(IntRect(0, 0, 1021, 871));
			dx = 0.1;
			health = 200;
			isShoot = false;
		}
		
	}
	void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
	{
		for (int i = y / 32; i < (y + h) / 32; i++)//проходимся по элементам карты
			for (int j = x / 32; j < (x + w) / 32; j++)
			{
				if (TileMap[i][j] == '0')//если элемент map.h
				{
					if(this->name == "EasyEnemy" || this->name == "MediumEnemy")
					{
						if (Dx > 0) { x = j * 32 - w; dx = -0.1; /*sprite.scale(-1, 1);*/ y += 10; }//с правым краем карты
						if (Dx < 0) { x = j * 32 + 32; dx = 0.1; /*sprite.scale(-1, 1);*/ y += 10; }// с левым краем карты
					}
					if (this->name == "HardEnemy")
					{
						if (Dx > 0) { x = j * 32 - w; dx = -0.1; }//с правым краем карты
						if (Dx < 0) { x = j * 32 + 32; dx = 0.1; }// с левым краем карты
					}
				}
			}
	}
	void update(float time) override
	{
		if (name == "EasyEnemy")
		{
			checkCollisionWithMap(dx, 0);
			x += dx * (time/1.1);
			sprite.setPosition(x + w / 2, y + h / 2);
			if (health <= 0) dead = true;
		}
		if (name == "MediumEnemy")
		{
			checkCollisionWithMap(dx, 0);
			x += dx * (time / 1.1);
			sprite.setPosition(x + w / 2, y + h / 2);			
			if (health <= 0) dead = true;
		}
		if (name == "HardEnemy")
		{
			checkCollisionWithMap(dx, 0);
			x += dx * (time / 1.1);
			sprite.setPosition(x + w / 2, y + h / 2);
			if (health <= 0) dead = true;
		}
		
	}
	float getEnemyXc()//c - Coordinate
	{
		return x;
	}
	float getEnemyYc()//c - Coordinate
	{
		return y;
	}
	
};
//----------------------------------класс пули------------------------------------------
class Bullet :public gameObj
{
public:
	int direction;
	Bullet(Image& image, String Name, float X, float Y, int W, int H, int dir) : gameObj(image, Name, X, Y, W, H)
	{
		x = X;
		y = Y;
		direction = dir;
		speed = 0.3;
		w = h = 16;
		dead = false;
	}
	Bullet(Image& image, String Name, float X, float Y, int W, int H) : gameObj(image, Name, X, Y, W, H)
	{
		x = X;
		y = Y;
		direction = 0.8;
		speed = -0.3;
		w = h = 16;
		dead = false;
	}
	void checkCollisionWithMap(float Dx, float Dy)//ф ция проверки столкновений с картой
	{
		for (int i = y / 32; i < (y + h) / 32; i++)//проходимся по элементам карты
		{
			for (int j = x / 32; j < (x + w) / 32; j++)
			{
				if (TileMap[i][j] == '0') dead = true;//смэрть пули, если ударилась об карту
			}
		}
	}
	void update(float time)
	{
		if (name == "Bullet")
		{
			switch (direction)
			{
			case 0: dx = 0; dy = -speed;   break;
			case 1: dx = 0; dy = -speed;   break;
			case 2: dx = 0; dy = -speed;   break;
			case 3: dx = 0; dy = -speed;   break;
			case 4: dx = 0; dy = -speed;   break;
			}
			y += dy * time;//по у
			if (y <= 0)y = 0;//задержка пули, чтобы при просадке кадров не вылетела за пределы допустимого
			sprite.setPosition(x + w / 2, y + h / 2);//задается позиция пуле
			if (y == 0) health -= 100;
			if (health <= 0) dead = true;
		}
		if (name == "EnemyBullet")
		{
			switch (direction)
			{
			case 0: dx = 0; dy = speed;   break;
			case 1: dx = 0; dy = speed;   break;
			case 2: dx = 0; dy = speed;   break;
			case 3: dx = 0; dy = speed;   break;
			case 4: dx = 0; dy = speed;   break;
			}
			y -= dy * time;//по у
			if (y >= 790)y = 790;//задержка пули, чтобы при просадке кадров не вылетела за пределы допустимого			
			sprite.setPosition(x + w / 2, y + h / 2);//задается позиция пуле
			if (y == 790) health -= 100;
			if (health <= 0) dead = true;
		}		
	}
};

int main()
{
	srand(time(NULL));
	setlocale(LC_ALL, "rus");

	RenderWindow window(VideoMode(1024, 860), "Space invaders project");
	menu(window);
	//--------------------------------шрифт и текст-----------------------------------------
	Font font;
	font.loadFromFile("images/madsense.ttf");
	Text text("", font, 20);
	text.setColor(Color::Magenta);
	text.setStyle(Text::Bold);
	Text text2("", font, 20);
	text2.setColor(Color::Magenta);
	text2.setStyle(Text::Bold);

	//--------------------------загрузка изображений----------------------------------------
	Image heroImage;//загрузка изображения игрока
	heroImage.loadFromFile("images/heroShip.png");
	
	Image easyEnemyImage;//загрузка изображения врага
	easyEnemyImage.loadFromFile("images/enemyEasy.png");
	Image mediumEnemyImage;//загрузка изображения врага
	mediumEnemyImage.loadFromFile("images/enemyMedium.png");
	Image hardEnemyImage;//загрузка изображения врага
	hardEnemyImage.loadFromFile("images/enemyHard.png");
	
	Image bulletImage, enemyBulletImage;
	bulletImage.loadFromFile("images/bulletHero.png");
	enemyBulletImage.loadFromFile("images/bulletEnemy.png");
	
	Image portalImage;
	portalImage.loadFromFile("images/portal.png");//загрузка портала	
	Texture portal;
	portal.loadFromImage(portalImage);//загрузка портала
	Sprite portSpace, portSpace2, portSpace3;
	portSpace.setTexture(portal);
	portSpace.setTextureRect(IntRect(30, 0, 350, 500));
	portSpace.setScale(0.15, 0.15);
	portSpace2.setTexture(portal);
	portSpace2.setTextureRect(IntRect(30, 0, 350, 500));
	portSpace2.setScale(0.15, 0.15);
	portSpace3.setTexture(portal);
	portSpace3.setTextureRect(IntRect(30, 0, 350, 500));
	portSpace3.setScale(0.3, 0.3);

	Texture expl;
	expl.loadFromFile("images/expl1.png");//загрузка вспышки взрыва
	Sprite explS;
	explS.setTexture(expl);
	explS.setTextureRect(IntRect(0, 0, 72, 79));
	explS.setPosition(900, 900);
	int Iexpl = 0;//счетчик для анимациии взрыва

		//------------------------------------------------------фон---------------------------------------------------

		Image map_space;//заполнение
		map_space.loadFromFile("images/space.png");
		Texture mapSpace;//текстура карты
		mapSpace.loadFromImage(map_space);
		Sprite space_map;
		space_map.setTexture(mapSpace);

		//------------------------------------------------создание объектов-------------------------------------

		Player player(heroImage, "Player1", 600, 600, 20.0, 20.0);
		player.sprite.setScale(0.12, 0.12);//размер спрайта

		//-----------------------------------------------привязка к времени------------------------------------

		Clock clock;
		Clock gameTimeClock;
		int gameTime = 0;
		float shootTime = 0;//время "перезарядки" врага
		float portalTime = 0;//время отрисовки портала
		float explTime = 0;//время отрисовки взрыва

		//-----------------------------------------набор противников и пуль--------------------------
		list<gameObj*> enemyList;
		list<gameObj*> bulletList;

		list <gameObj*>::iterator itE;
		list<gameObj*>::iterator itB;

		int coordX = 50, coordY = 50;//для сдвига координаты Х

		//--------------------------------------------------------------работа в окне----------------------------------------

		while (window.isOpen())//пока открыто окно-выводим изображение
		{
			//привязка к времени
			float time = clock.getElapsedTime().asMicroseconds();
			if (player.dead) gameTime = gameTimeClock.getElapsedTime().asSeconds();//если игрок жив, то игровое время обновляется
			clock.restart();
			time = time / 550;//скорость игры (чем меньше делитель-тем медленнее)
			if (player.dead == true)
			{
				restartMenu(window);
				window.clear();
				gameTime = 0;
				//clock.restart();
			}

			Event event;
			while (window.pollEvent(event))
			{
				if (event.type == Event::Closed)
					window.close();
				if (event.key.code == Keyboard::Space)
					bulletList.push_back(new Bullet(bulletImage, "Bullet", player.getPlayerXc(), player.getPlayerYc(), 20, 40, player.state));
				if (event.key.code == Keyboard::Escape)
					gameMenu(window, player.dead);
			}

			//вывод на экран
			window.clear();

			//----------------------------------------------------------отрисовка фона---------------------------------------------------------------

			space_map.setTextureRect(IntRect(0, 0, 1024, 860));
			window.draw(space_map);//рисуем фон
			player.update(time);

			//-------------------------------------------------------обновление списка противников-------------------------------------------------
			if (enemyList.empty())
			{
				coordX = 50 + rand() % 600, coordY = 50 + rand() % 200;
				portSpace.setPosition(coordX, coordY);
				for (size_t i = 0; i < 1 + rand() % 5; i++)
				{
					enemyList.push_back(new Enemy(easyEnemyImage, "EasyEnemy", coordX, coordY, 25.0, 25.0));
					coordX += 70;
				}
				coordX = 50 + rand() % 600, coordY = 50 + rand() % 200;
				portSpace2.setPosition(coordX, coordY);
				for (size_t i = 0; i < 1 + rand() % 5; i++)
				{
					enemyList.push_back(new Enemy(mediumEnemyImage, "MediumEnemy", coordX, coordY, 25.0, 25.0));
					coordX += 70;
				}
			}
			if (player.playerScore > 1000)
			{
				portSpace3.setPosition(500, 50);
				enemyList.push_back(new Enemy(mediumEnemyImage, "HardEnemy", 500, 50, 50.0, 50.0));
			}
			//--------------------------------------------------смэрть объектов при пересечении друг друга-------------------------------------------

			for (itE = enemyList.begin(); itE != enemyList.end(); itE++)
			{
				if ((*itE)->getRect().intersects(player.getRect()))
				{
					if ((*itE)->name == "EasyEnemy" || (*itE)->name == "MediumEnemy")
					{
						player.dead = true;
					}
				}
				if (!bulletList.empty())
				{
					if ((*itE)->getRect().intersects(bulletList.front()->getRect()))
					{
						if ((*itE)->name == "EasyEnemy" && bulletList.front()->name != "EnemyBullet" || (*itE)->name == "MediumEnemy" && bulletList.front()->name != "EnemyBullet") //отключаем френдли фаер
						{
							(*itE)->health -= 100;
						}
					}
				}
			}
			for (itB = bulletList.begin(); itB != bulletList.end(); itB++)
			{
				if ((*itB)->getRect().intersects(player.getRect()))
				{
					if ((*itB)->name == "EnemyBullet")
					{
						player.health -= 100;
					}
				}
			}
			
			//-------------------------------------------------------стрельба противника и "очистки" экрана от ненужных спрайтов-----------------------------------------------------------
			explTime += time;
			portalTime += time;
			shootTime += time;
			if (!enemyList.empty())
			{
				if (shootTime > 3000)
				{
					for (itE = enemyList.begin(); itE != enemyList.end(); itE++)
					{
						if ((*itE)->name == "MediumEnemy") bulletList.push_back(new Bullet(enemyBulletImage, "EnemyBullet", (*itE)->x, (*itE)->y, 20, 40));
					}
					shootTime = 0;
				}
				if (portalTime > 700)
				{
					portSpace.setPosition(900, 900);
					portSpace2.setPosition(900, 900);
					portalTime = 0;
				}
				if (explTime > 500)
				{
					explS.setPosition(950, 950);
					explTime = 0;
				}
			}

			//--------------------------------------------------------------------вывод объектов на экран-----------------------------------------
			for (itE = enemyList.begin(); itE != enemyList.end(); itE++) { (*itE)->update(time); }
			for (itE = enemyList.begin(); itE != enemyList.end(); itE++)
			{
				if ((*itE)->name == "EasyEnemy")(*itE)->sprite.setScale(0.05, 0.05);
				if ((*itE)->name == "MediumEnemy")(*itE)->sprite.setScale(0.06, 0.06);
				window.draw((*itE)->sprite);	
			}
			for (itB = bulletList.begin(); itB != bulletList.end(); itB++) { (*itB)->update(time); }
			for (itB = bulletList.begin(); itB != bulletList.end(); itB++)
			{
				if ((*itB)->name == "Bullet")(*itB)->sprite.setScale(0.4, 0.4);
				if ((*itB)->name == "EnemyBullet")(*itB)->sprite.setScale(0.9, 0.9);
				window.draw((*itB)->sprite);
			}
			
			//---------------------------------------------удаление объекта из списка,начисление очков-----------------------------------------------
			for (itE = enemyList.begin(); itE != enemyList.end();)
			{
				gameObj* b = *itE;
				b->update(time);
				if (b->dead == true)
				{
					if (b->name == "EasyEnemy") player.playerScore += 25;
					if (b->name == "MediumEnemy") player.playerScore += 50;

					itE = enemyList.erase(itE);

					explS.setPosition(b->x, b->y);
					Iexpl++;
					switch (Iexpl)//анимация взрыва
					{
					case 1:
						expl.loadFromFile("images/expl1.png");
						break;
					case 2:
						expl.loadFromFile("images/expl2.png");
						break;
					case 3:
						expl.loadFromFile("images/expl3.png");
						break;
					case 4:
						expl.loadFromFile("images/expl4.png");
						break;
					case 5:
						expl.loadFromFile("images/expl5.png");
						break;
					case 6:
						expl.loadFromFile("images/expl6.png");
						break;
					default:
						Iexpl = 0;
						break;
					}
					delete b;
				}
				else itE++;
			}
			window.draw(explS);
			for (itB = bulletList.begin(); itB != bulletList.end();)
			{
				gameObj* b = *itB;
				b->update(time);
				if (b->dead == true)
				{
					itB = bulletList.erase(itB);
					delete b;
				}
				else itB++;
			}

			//------------------------------------------------------------------вывод спрайтов и отрисовка текста--------------------------------------
			window.draw(portSpace);//рисуем портал
			window.draw(portSpace2);//рисуем портал
			
			window.draw(player.sprite);//вывод спрайта класса Player на экран

			ostringstream scoreString, gameTimeString;//вывод очков игрока и времени игры
			scoreString << player.playerScore; gameTimeString << gameTime;
			text.setString("Time " + gameTimeString.str());
			text.setPosition(10.0, 10.0);
			window.draw(text);
			text2.setString("Score " + scoreString.str());
			text2.setPosition(910.0, 10.0);
			window.draw(text2);

			
			window.display();
		}
	
	


	return 0;
	

}




