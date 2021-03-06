#include <algorithm>
#include <fstream>

#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "soccer/Match.h"
#include "soccer/Team.h"
#include "soccer/DataExchange.h"
#include "soccer/gui/Menu.h"
#include "soccer/gui/CupScreen.h"

namespace Soccer {

CupScreen::CupScreen(boost::shared_ptr<ScreenManager> sm, boost::shared_ptr<StatefulCup> l,
		bool onlyOneRound)
	: CompetitionScreen(sm, "Cup", l, onlyOneRound),
	mCup(l)
{
	updateScreenElements();
}

void CupScreen::saveCompetition(boost::archive::binary_oarchive& oa) const
{
	oa << mCup;
}

}


