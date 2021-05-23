#include <iostream>

#include "Player.h"

namespace BlackJack
{

	Player::Player(std::string name)
		: Name(name), 
		  Hand(BlackJack::Hand()), 
		  Ante(0), 
		  Cash(1000),
		  HasCurrentTurn(false)
	{
	}

	void Player::PrintHand()
	{
		Hand.Print();
	}

	bool Player::IsBusted()
	{
		return Hand.IsBusted();
	}

	Hand Player::GetHand()
	{
		return Hand;
	}

	bool Player::WillHit()
	{
		std::string hit;
		std::cout << "Hit?" << std::endl;
		std::cin >> hit;
		return (hit == "y");
	};

	void Player::TakeCard(Card& card)
	{
		Hand.push_back(card);
	};

	uint8_t Player::GetHandValue()
	{
		return Hand.GetValue();
	}

	void Player::DiscardHand()
	{
		this->Hand.Discard();
	};

	void Player::SetAnte(uint32_t ante)
	{
		this->Ante = ante;
	};

	uint32_t Player::GetAnte()
	{
		return Ante;
	};

	std::string Player::GetName()
	{
		return Name;
	};

	void Player::AddCash(uint32_t cash)
	{
		this->Cash += cash;
	};

	uint32_t Player::GetCash()
	{
		return this->Cash;
	};


	bool Player::operator==(const Player& player)
	{
		return this->Name == player.Name;
	};

}

