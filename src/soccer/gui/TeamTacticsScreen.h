#ifndef SOCCER_TEAMTACTICSSCREEN_H
#define SOCCER_TEAMTACTICSSCREEN_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "soccer/Match.h"

#include "soccer/gui/Screen.h"

namespace Soccer {

class TeamTacticsScreenOwner {
	public:
		~TeamTacticsScreenOwner() { }
		virtual void TeamTacticsScreenFinished(int playernum) = 0;
};

class TeamTacticsScreen : public Screen {
	public:
		TeamTacticsScreen(boost::shared_ptr<ScreenManager> sm, Match& m,
				TeamTacticsScreenOwner& ttso);
		void buttonPressed(boost::shared_ptr<Button> button);
		const std::string& getName() const;

	private:
		void setupTeamDisplay(int i);
		void setupPlrLabels();
		void addFormationButton(int i, int formbutton, const std::string& str);
		void addPlayerLabels(const boost::shared_ptr<Player> pl, int i, float x,
				float y, float namewidth, float nameheight);
		static const std::string ScreenName;
		Match& mMatch;
		TeamTacticsScreenOwner& mTtso;
		int mChosenplnum;
		bool mShowHome;
		int mHumanTeam;
		Common::Rectangle mPitchRect;
		std::map<boost::shared_ptr<Button>, int> mPlayers[2];
		boost::shared_ptr<Button> mToggleButtons[2];
		boost::shared_ptr<Button> mTeamLabels[2];
		std::vector<boost::shared_ptr<Button>> mPlayerLabels[2];
		std::vector<boost::shared_ptr<Button>> mFormationButtons[2];
		std::string mFormationNumbers[2];
		std::vector<boost::shared_ptr<Button>> mSkillLabels[2];
		std::map<boost::shared_ptr<Button>, int> mPlayerControllerButtons[2];
		std::vector<boost::shared_ptr<Button>> mTop3Labels[2];
		std::pair<boost::shared_ptr<Button>, int> mSelectedPlayer;

		std::vector<boost::shared_ptr<Slider>> mTacticsSliders[2];
};

}

#endif



