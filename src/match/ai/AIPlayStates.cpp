#include "match/PlayerActions.h"
#include "match/ai/AIPlayStates.h"
#include "match/ai/PlayerAIController.h"
#include "match/MatchHelpers.h"
#include "match/ai/AIHelpers.h"

AIPlayController::AIPlayController(Player* p)
	: PlayerController(p)
{
	if(p->isGoalkeeper())
		mCurrentState = boost::shared_ptr<AIState>(new AIGoalkeeperState(p, this));
	else
		mCurrentState = boost::shared_ptr<AIState>(new AIDefendState(p, this));
}

boost::shared_ptr<PlayerAction> AIPlayController::act(double time)
{
	if(mPlayer->getMatch()->getBall()->grabbed()) {
		if(mPlayer->getMatch()->getBall()->getGrabber() == mPlayer) {
			return mCurrentState->actOnBall(time);
		}
		else {
			return actOnRestart(time);
		}
	}
	else {
		bool nearest = mPlayer->getTeam()->getPlayerNearestToBall() == mPlayer;
		bool cankick = MatchHelpers::canKickBall(*mPlayer);
		if(cankick)
			return mCurrentState->actOnBall(time);
		else if(nearest)
			return mCurrentState->actNearBall(time);
		else
			return mCurrentState->actOffBall(time);
	}
}

boost::shared_ptr<PlayerAction> AIPlayController::switchState(boost::shared_ptr<AIState> newstate, double time)
{
	mCurrentState = newstate;
	return act(time);
}

void AIPlayController::setNewState(boost::shared_ptr<AIState> newstate)
{
	mCurrentState = newstate;
}

const std::string& AIPlayController::getDescription() const
{
	return mCurrentState->getDescription();
}

boost::shared_ptr<PlayerAction> AIPlayController::actOnRestart(double time)
{
	const Player* restarter;
	if(MatchHelpers::myTeamInControl(*mPlayer)) {
		restarter = MatchHelpers::nearestOwnPlayerToBall(*mPlayer->getTeam());
	}
	else {
		restarter = MatchHelpers::nearestOppositePlayerToBall(*mPlayer->getTeam());
	}

	if(!MatchHelpers::myTeamInControl(*mPlayer) &&
			mPlayer->getMatch()->getPlayState() == PlayState::OutPenaltykick &&
			mPlayer->isGoalkeeper()) {
		return AIHelpers::createMoveActionTo(*mPlayer,
				MatchHelpers::ownGoalPosition(*mPlayer));
	}

	if(!MatchHelpers::myTeamInControl(*mPlayer) &&
			mPlayer->getMatch()->getPlayState() == PlayState::OutDirectFreekick &&
			!mPlayer->isGoalkeeper() && mPlayer->getShirtNumber() >= 8) {
		Vector3 ballpos = mPlayer->getMatch()->getBall()->getPosition().v;
		Vector3 goalpos = MatchHelpers::ownGoalPosition(*mPlayer).v;
		if((ballpos - goalpos).length() < 35.0f) {
			Vector3 mypos = (goalpos - ballpos).normalized() * 9.50f;

			Vector3 perp;
			perp.x = -mypos.y;
			perp.y = mypos.x;
			perp.normalize();
			float dist = 0.8f * (mPlayer->getShirtNumber() - 10);

			mypos += perp * dist;

			return AIHelpers::createMoveActionTo(*mPlayer,
					AbsVector3(ballpos + mypos));
		}
	}

	if(!MatchHelpers::playerPositionedForRestart(*restarter, *mPlayer)) {
		AbsVector3 dir = MatchEntity::vectorFromTo(*mPlayer->getMatch()->getBall(),
				*mPlayer);

		if(dir.v.null() || mPlayer->getMatch()->getPlayState() == PlayState::OutPenaltykick) {
			dir = mPlayer->getMatch()->getBall()->getPosition();
			dir.v *= -1.0f;
		}
		dir.v.normalize();

		return AIHelpers::createMoveActionTo(*mPlayer,
				AbsVector3(mPlayer->getPosition().v + dir.v));
	}
	else {
		return mCurrentState->actOffBall(time);
	}
}

void AIPlayController::matchHalfChanged(MatchHalf m)
{
	mCurrentState->matchHalfChanged(m);
}

AIState::AIState(Player* p, AIPlayController* m)
	: mPlayer(p),
	mPlayController(m)
{
}

boost::shared_ptr<PlayerAction> AIState::actOnBall(double time)
{
	mDescription = std::string("Preparing kick");
	return mPlayController->switchState(boost::shared_ptr<AIState>(new AIKickBallState(mPlayer, mPlayController)), time);
}

boost::shared_ptr<PlayerAction> AIState::actNearBall(double time)
{
	mDescription = std::string("Fetching");
	return AIHelpers::createMoveActionToBall(*mPlayer);
}

boost::shared_ptr<PlayerAction> AIState::switchState(boost::shared_ptr<AIState> newstate, double time)
{
	return mPlayController->switchState(newstate, time);
}

void AIState::setNewState(boost::shared_ptr<AIState> newstate)
{
	mPlayController->setNewState(newstate);
}

const std::string& AIState::getDescription() const
{
	return mDescription;
}


