#include "BlackJackPlayer.h"

void BlackJackPlayer::SetLastAction()
{
	tLastAction = std::chrono::system_clock::now();
}

double BlackJackPlayer::GetTimeSinceLastAction()
{
	std::chrono::duration<double> tElapsed = std::chrono::system_clock::now() - tLastAction;
	return tElapsed.count();
}


BlackJackPlayer::BlackJackPlayer()
	: BlackJack::Player()
{
	SetLastAction();
};

