#pragma once

enum class DIRECTION : BYTE
{
	LEFT = 0,
	UP = 1,
	RIGHT = 2,
	DOWN = 3,
	DIRECTION_END
};

struct Position2D 
{
	UINT8 x;
	UINT8 y;

	Position2D(const UINT8 inPositionX = 0, const UINT8 inPositionY = 0) noexcept
		: x(inPositionX), y(inPositionY)
	{}
};

class UserData 
{
	Position2D position;

public:
	UserData(const UINT8 inPositionX = 0, const UINT8 inPositionY = 0) noexcept;

	UserData() = delete;
	~UserData() = default;

public:
	void MoveCharacter(const DIRECTION);
	
public:
	inline Position2D GetPosition() const noexcept { return position; }
	/*Not Safe*/ inline Position2D& RefPosition() noexcept { return position; }
};