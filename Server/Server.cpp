#include "pch.h"
#include "GameServer.h"

int main(int argc, char * argv[])
{
	std::unique_ptr<GameServer> gameServer
		= std::make_unique<GameServer>
		(
			[/* void */]() noexcept(false) -> bool
			{
#ifdef _DEBUG_MODE_
				std::cout << " 주의!! _DEBUG_MODE_가 활성화 되어 있습니다. " << std::endl;
#endif
				return true;
			}()
		);

	gameServer->Run();
}
