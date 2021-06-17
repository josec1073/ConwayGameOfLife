#include "Board.h"
#include <random>

Board::Cell::Cell(const sf::Vector2f& pos, float size, bool isAlive)
	:
	size(size),
	isAlive(isAlive)
{
	cell.setPosition(pos);
	cell.setSize({ size, size });
	cell.setOutlineThickness(-1.0f);
	cell.setFillColor(sf::Color::White);
	cell.setOutlineColor(sf::Color::Black);
}

void Board::Cell::Revive()
{
	isAlive = true;
}

void Board::Cell::Kill()
{
	isAlive = false;
}

int Board::Cell::CountAliveNeighbors(const Board& board) const
{
	const sf::Vector2f pos = cell.getPosition();
	const sf::Vector2i indexPos = static_cast<sf::Vector2i>((pos - board.GetPosition()) / size);
	const int boardWidth = board.GetWidth();
	const int boardHeight = board.GetHeight();
	int n = 0;

	for (int i = indexPos.x - 1; i <= indexPos.x + 1; ++i)
	{
		for (int j = indexPos.y - 1; j <= indexPos.y + 1; ++j)
		{
			if (i != indexPos.x || j != indexPos.y)
			{
				if (i >= 0 && i < boardWidth && j >= 0 && j < boardHeight)
				{
					if (board.lifeCells[i][j] == true)
					{
						n++;
					}
				}
			}
		}
	}

	return n;
}

void Board::Cell::Render(sf::RenderTarget& target)
{
	if (isAlive)
	{
		target.draw(cell);
	}
}

void Board::Cell::Update(const Board& board)
{
	const int numNeighbors = CountAliveNeighbors(board);
	if (isAlive)
	{
		if (numNeighbors != 2 && numNeighbors != 3)
		{
			isAlive = false;
		}
	}
	else
	{
		if (numNeighbors == 3)
		{
			isAlive = true;
		}
	}
}

Board::Board(const sf::Vector2f& pos, float cellSize, int width, int height, bool randomGenerated)
	:
	pos(pos),
	cellSize(cellSize),
	width(width),
	height(height),
	cells(width, std::vector<Cell>(height, Cell())),
	lifeCells(width, std::vector<bool>(height, false))
{

	border.setPosition(pos);
	border.setSize(sf::Vector2f(cellSize * width, cellSize * height));
	border.setFillColor(sf::Color::Transparent);
	border.setOutlineThickness(borderSize);
	border.setOutlineColor(sf::Color(120, 120, 120));

	if (randomGenerated)
	{
		std::random_device rd;
		std::mt19937 random(rd());
		std::uniform_int_distribution<int> randomizer(0, 1);

		for (int i = 0; i < width; ++i)
		{
			for (int j = 0; j < height; ++j)
			{
				const int val = randomizer(random);
				cells[i][j] = Cell(sf::Vector2f(pos.x + i * cellSize, pos.y + j * cellSize), cellSize, val);
			}
		}
	}
	else
	{
		for (int i = 0; i < width; ++i)
		{
			for (int j = 0; j < height; ++j)
			{
				cells[i][j] = Cell(sf::Vector2f(pos.x + i * cellSize, pos.y + j * cellSize), cellSize, false);
			}
		}
	}
	
}

void Board::Render(sf::RenderTarget& target)
{
	target.draw(border);

	for (auto& cell : cells)
	{
		for (auto& cel : cell)
		{
			cel.Render(target);
		}
	}
}

void Board::Update()
{
	CopyState();
	for (auto& cell : cells)
	{
		for (auto& cel : cell)
		{
			cel.Update(*this);
		}
	}
}

void Board::UpdateInput(const sf::Vector2i& mousePos)
{
	const sf::FloatRect boardBounds = sf::FloatRect(0.0f, 0.0f, width * cellSize, height * cellSize);
	const sf::Vector2i gridPos = sf::Vector2i(int(mousePos.x - pos.x/ cellSize), int(mousePos.y - pos.y / cellSize));

	if (boardBounds.contains(static_cast<sf::Vector2f>(mousePos)))
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			cells[gridPos.x][gridPos.y].Revive();
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
		{
			cells[gridPos.x][gridPos.y].Kill();
		}
	}
	
}

void Board::CopyState()
{
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			lifeCells[i][j] = cells[i][j].IsAlive();
		}
	}
}
