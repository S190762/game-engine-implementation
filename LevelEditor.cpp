#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <vector>

class Tile {
	sf::Texture* texture;
	sf::Texture tiletex;
	sf::IntRect textureRect;
public:
	Tile(sf::Texture* tex, int texX, int texY, int tileSize) {
		texture = tex;
		textureRect = sf::IntRect(texX, texY, tileSize, tileSize);
	}
	sf::Vector2f getTexCoord() {
		return sf::Vector2f(textureRect.left, textureRect.top);
	}
	sf::IntRect getTexRect() {
		return textureRect;
	}
	sf::Texture getTexture() {
		return *texture;
	}
};


Tile* defaultTileImage;


class MapTile : public sf::Transformable, public sf::Drawable {
	sf::Sprite tile;
	int posX;
	int posY;
	Tile* currentTileImage;
public:
	MapTile(int x, int y) {
		tile = sf::Sprite();
		tile.setPosition(x, y);
		currentTileImage = defaultTileImage;
		tile.setTexture(currentTileImage->getTexture());
		tile.setTextureRect(currentTileImage->getTexRect());
	}
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		target.draw(tile, states);
	}
	void ChangeTileImage(Tile* selected) {
		currentTileImage = selected;
	}
	bool TileCheck(int x, int y) {
		return tile.getGlobalBounds().contains(x, y);
	}
};

class TileSet {
	std::vector<Tile*> tiles;
	sf::Texture texture;
public:
	TileSet(std::string filename, int row, int col, int tileSize) {
		texture.loadFromFile(filename);
		for(size_t r = 0; r < row; r++)
			for(size_t c = 0; c < col; c++)
				AddTile(new Tile(&texture, r, c, tileSize));
	}
	sf::Texture* GetTexture() {
		return &texture;
	}
	Tile* GetTile(int texX, int texY) {
		for(auto tile : tiles)
			if(tile->getTexCoord().x == texX && tile->getTexCoord().y == texY)
				return tile;
	}
	void AddTile(Tile* tile) {
		tiles.push_back(tile);
	}
};

class TileMapLayer  : public sf::Transformable, public sf::Drawable {
	std::vector<MapTile*> tiles;
public:
	void AddTile(MapTile* tile) {
		tiles.push_back(tile);
	}
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		for(auto tile : tiles)
			target.draw(*tile, states);
	}
};

class TileMap : public sf::Transformable, public sf::Drawable {
	std::vector<TileMapLayer*> layers;
public:
	void AddTileLayer(TileMapLayer* tml) {
		layers.push_back(tml);
	}
	void AddTileAtLayer(MapTile* tile, TileMapLayer* tilemaplayer) {
		tilemaplayer->AddTile(tile);
	}
	void draw(sf::RenderTarget& target, sf::RenderStates states) const {
		for(auto layer : layers)
			target.draw(*layer, states);
	}
};

int main() {
	sf::RenderWindow render_window( sf::VideoMode( 1024,768 ), "Example" );
	TileMap map;
	TileSet tileset("tileset.png", 5, 5, 64);

	defaultTileImage = tileset.GetTile(2, 0);

	TileMapLayer ground;
	TileMapLayer layer1;
	TileMapLayer layer2;

	map.AddTileLayer(&ground);
	map.AddTileLayer(&layer1);
	map.AddTileLayer(&layer2);

	for(int row = 0; row < 15; row++)
	{
		for (int col = 0; col < 15; col++)
		{
			ground.AddTile(new MapTile(row, col));
		}
	}

	sf::Event event;

	while( render_window.isOpen() )
	{
		while( render_window.pollEvent( event ) )
		{
			if (event.type == sf::Event::Closed)
			{
				return 0;
			}
		}
		render_window.clear();
		render_window.draw(map);
		render_window.display();
	}
}