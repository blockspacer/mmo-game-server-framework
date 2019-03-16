#include "pch.h"
#include "GameServer.h"

int main(int argc, char * argv[])
{
	std::unique_ptr<GameServer> gameServer
		= std::make_unique<GameServer>
		(
			//[/* void */]() noexcept(false) -> const SERVER_IP_TYPE
			//{
			//}()
		);

	gameServer->Run();
}
