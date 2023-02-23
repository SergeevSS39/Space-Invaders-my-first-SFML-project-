#include <SFML\Graphics.hpp>
using namespace sf;


void gameMenu(RenderWindow& window, bool dead)
{
	Font font;
	font.loadFromFile("images/madsense.ttf");
	Text banner("", font, 60);
	banner.setColor(Color::Magenta);
	banner.setStyle(Text::Bold);
	Text menu1("", font, 35);
	menu1.setColor(Color::Magenta);
	menu1.setStyle(Text::Bold);
	Text menu2("", font, 35);
	menu2.setColor(Color::Magenta);
	menu2.setStyle(Text::Bold);

	Texture menuBackground;
	menuBackground.loadFromFile("images/space.png");
	Sprite menuBg(menuBackground);

	bool isMenu = true;
	int menuNum = 0;

	banner.setPosition(380, 100);
	menu1.setPosition(450, 300);
	menu2.setPosition(450, 360);

	menuBg.setPosition(0, 0);

	//------------------------------------------------игровое меню-------------------------------
	while (isMenu)
	{
		menu1.setColor(Color::Magenta);
		menu2.setColor(Color::Magenta);
		
		menuNum = 0;
		window.clear(Color(129, 181, 221));

		if (IntRect(450, 300, 300, 50).contains(Mouse::getPosition(window))) { menu1.setColor(Color::Blue); menuNum = 1; }
		if (IntRect(450, 360, 300, 50).contains(Mouse::getPosition(window))) { menu2.setColor(Color::Blue); menuNum = 2; }

		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (menuNum == 1) { isMenu = false; }
			if (menuNum == 2) { window.close(); isMenu = false; }
		}
		
		banner.setString("  Space\nInvaders");
		menu1.setString("return");
		menu2.setString("exit");
		

		window.draw(menuBg);
		window.draw(banner);
		window.draw(menu1);
		window.draw(menu2);

		window.display();
	}


}
