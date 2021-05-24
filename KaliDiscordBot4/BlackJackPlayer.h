#include <thread>

#include <BlackJack/Player.h>


class BlackJackPlayer : public BlackJack::Player
{
public:
	bool HasCurrentTurn;
	std::chrono::seconds tLastAction;

	void SetLastAction();
	double GetTimeSinceLastAction();

	BlackJackPlayer();

private:

};
