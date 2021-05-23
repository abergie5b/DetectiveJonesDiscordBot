#ifndef __CARD_H_
#define __CARD_H_

#include "Types.h"

namespace BlackJack
{

	class Card
	{
		public:
			Card();
			Card(const Card&);
			Card& operator=(const Card&);
			Card(CardNames, SuiteNames);
			const CardNames GetName() const;
			const SuiteNames GetSuite() const;
			const uint8_t GetHighValue() const;
			const uint8_t GetLowValue() const;
		private:
			CardNames name;
			SuiteNames suite;
			uint8_t highvalue;
			uint8_t lowvalue;
	};

}

#endif
