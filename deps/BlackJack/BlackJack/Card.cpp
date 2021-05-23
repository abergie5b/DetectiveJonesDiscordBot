#include "Card.h"

namespace BlackJack
{

	Card::Card()
		: name(CardNames::NONE), 
		  suite(SuiteNames::NONE), 
		  highvalue(0), 
		  lowvalue(0)
	{
	}

	Card::Card(const Card& card)
		: name(card.name), 
		  suite(card.suite), 
		  highvalue(card.highvalue), 
		  lowvalue(card.lowvalue)
	{
	}

	Card& Card::operator=(const Card& card)
	{
		name = card.name;
		suite = card.suite;
		highvalue = card.highvalue;
		lowvalue = card.lowvalue;
		return *this;
	}

	Card::Card(CardNames name, SuiteNames suite)
	{
		this->name = name;
		this->suite = suite;
		if (name == CardNames::ACE)
		{
			highvalue = 11;
			lowvalue = 1;
		}
		else if (name > CardNames::TEN)
			highvalue = lowvalue = 10;
		else
			highvalue = lowvalue = (uint8_t)name;
	}

	const CardNames Card::GetName() const
	{
		return name;
	};

	const SuiteNames Card::GetSuite() const
	{
		return suite;
	};

	const uint8_t Card::GetLowValue() const
	{
		return lowvalue;
	};


	const uint8_t Card::GetHighValue() const
	{
		return highvalue;
	};

}

