#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cmath>

int clamp(int val, int min, int max) {
	val = val < min ? min : val;
	val = val > max ? max : val;
	return val;
}

bool pointInRectangle(int x, int y, int x1, int y1, int x2, int y2)
{
	return (clamp(x, x1, x2) == x && clamp(y, y1, y2) == y);
}

std::vector<std::vector<sf::Texture*>> tileset;
int selectedX = 1;
int selectedY = 1;
int layer = 0;
sf::RenderTexture gametexture;
sfg::Desktop desktop;
sfg::Image::Ptr gameimage;
sfg::Label::Ptr currentlayer;

struct Tile : public sf::Transformable, public sf::Drawable {
	sf::RectangleShape* rs = new sf::RectangleShape;
	int posX;
	int posY;
	int currentTileX;
	int currentTileY;
	Tile(int x, int y) {
		posX = x * 64;
		posY = y * 64;
		rs->setSize({64, 64});
		rs->setPosition(posX, posY);
		rs->setTexture(tileset[1][1]);
		currentTileX = 1;
		currentTileY = 1;
	}
	void click() {
		std::cout << "current: " << layer << " x " << selectedX << " y " << selectedY << std::endl;
		rs->setTexture(tileset[selectedX][selectedY]);
		currentTileX = selectedX;
		currentTileY = selectedY;
	}
	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(*rs, states);
	}
	bool withinTile(int x, int y) {
		return rs->getGlobalBounds().contains(x, y);
	}
};

void increaseLayer() {
	layer++;
}

sf::Image resizeImage(sf::Texture* p_image)
{
	sf::Sprite spriteTmp(*p_image);
	spriteTmp.scale(0.25, 0.25);
	sf::RenderTexture image;
	image.create(16, 16);
	image.clear(sf::Color(0,0,0,255));
	image.draw(spriteTmp);
	image.display();
	return image.getTexture().copyToImage();
}

void draw_gui() {
	desktop.LoadThemeFromFile( "theme.theme" );
	auto SFMLview = sfg::Window::Create();
	gameimage = sfg::Image::Create();
	SFMLview->SetPosition({50, 50});
	SFMLview->SetTitle("SFMLView");
	SFMLview->Add(gameimage);
	desktop.Add(SFMLview);
	auto tilesetw = sfg::Window::Create();
	tilesetw->SetPosition({700, 50});
	tilesetw->SetTitle("Tileset");
	auto layerb = sfg::Button::Create("next layer");
	layerb->GetSignal(sfg::Button::OnLeftClick).Connect(increaseLayer);

	currentlayer = sfg::Label::Create();

	auto tilesetwb = sfg::Table::Create();
	for(int row = 0; row < tileset.size(); ++row)
	{
		for (int col = 0; col < tileset[row].size(); ++col)
		{
			auto temp = sfg::Button::Create();
			auto tempi = sfg::Image::Create(resizeImage(tileset[row][col]));
			tempi->SetRequisition({16.0f, 16.0f});
			temp->SetImage(tempi);
			temp->GetSignal(sfg::Button::OnLeftClick).Connect([row, col]() {
				selectedY = (int) col;
				selectedX = (int) row;
			});
			tilesetwb->Attach(temp, sf::Rect<sf::Uint32>(col, row, 1, 1));
		}
	}

	auto tilesetl = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 0.0f);
	tilesetl->Pack(currentlayer);
	tilesetl->Pack(layerb);
	tilesetl->Pack(tilesetwb);

	tilesetw->Add(tilesetl);
	desktop.Add(tilesetw);
}

int main() {
	sf::RenderWindow render_window( sf::VideoMode( 1024,768 ), "CKEngine v0.1 - Map Editor" );
	sfg::SFGUI sfgui;
	sf::Event event;
	sf::Clock clock;
	sf::View gameView;
	gametexture.create(640, 480);

	sf::RectangleShape gametextures;
	gametextures.setSize({640, 480});
	gametextures.setTexture(&gametexture.getTexture());
	gametextures.setPosition(50, 50);

	gameView.setSize(640, 480);

	sf::RectangleShape background({1024, 768});
	sf::Texture backgroundtext;
	backgroundtext.loadFromFile("editor_background.png");
	background.setTexture(&backgroundtext);
	background.setPosition(0, 0);

	std::vector<std::vector<std::vector<Tile>>> tilemap;

	for(size_t row = 0; row < 15; row++) {
		tileset.push_back(std::vector<sf::Texture*>());
		for(size_t col = 0; col < 15; col++) {
			sf::Texture* temp = new sf::Texture;
			temp->loadFromFile("tileset.png", {row * 64, col * 64, 64, 64});
			tileset[row].push_back(temp);
		}
	}

	for(size_t layer = 0; layer < 2; layer++)
	{
		tilemap.push_back(std::vector<std::vector<Tile>>());
		for (size_t row = 0; row < 11; row++)
		{
			tilemap[layer].push_back(std::vector<Tile>());
			for (size_t col = 0; col < 15; col++)
			{
				tilemap[layer][row].push_back(Tile((int) col, (int) row));
			}
		}
	}

	draw_gui();

	render_window.resetGLStates();

	gameView.setCenter(render_window.getSize().x / 2,
					   render_window.getSize().y / 2);

	while( render_window.isOpen() ) {
		while( render_window.pollEvent( event ) ) {
			desktop.HandleEvent( event );
			if( event.type == sf::Event::Closed ) {
				return 0;
			}
			if(event.type == sf::Event::KeyPressed) {
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
					gameView.move(50, 0);
				}
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
					gameView.move(0, 50);
				}
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
					gameView.move(0, -50);
				}
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
					gameView.move(-50, 0);
				}
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1)) {
					selectedX = 1;
				}
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2)) {
					selectedX = 2;
				}
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3)) {
					selectedX = 3;
				}
			}
			if(event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Button::Left)
				{
					sf::Vector2f worldPos = gametexture.mapPixelToCoords({sf::Mouse::getPosition(render_window).x - gameimage->GetAbsolutePosition().x,
																		sf::Mouse::getPosition(render_window).y - gameimage->GetAbsolutePosition().y});
					int posX = worldPos.x;
					int posY = worldPos.y;
					for (auto row : tilemap[layer])
						for (auto col : row)
							if(col.withinTile(posX, posY))
								col.click();
				}
			}
		}

		desktop.Update( clock.restart().asSeconds() );

		currentlayer->SetText(std::string("current layer: " + std::to_string(layer)));

		gametexture.setView(gameView);
		gametexture.clear();
		for(auto layer : tilemap)
			for(auto row : layer)
				for(auto col : row)
					gametexture.draw(col);
		gametexture.display();

		gameimage->SetImage(gametexture.getTexture().copyToImage());

		render_window.clear(sf::Color(31, 34, 36));
		render_window.draw(background);
		sfgui.Display( render_window );
		render_window.display();
	}



	return 0;
}
