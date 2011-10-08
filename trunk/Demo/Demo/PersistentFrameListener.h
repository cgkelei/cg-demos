#ifndef PERSISTENTFRAMELISTENER_H_
#define PERSISTENTFRAMELISTENER_H_

#include <Ogre.h>
#include "ShipGame.h"

using namespace Ogre;

class PersistentFrameListener :
	public FrameListener
{
public:
	PersistentFrameListener() :
		isStarted(false)
	{
		ShipGame::getSingleton().GetRoot()->addFrameListener(this);
	}
	
	virtual ~PersistentFrameListener()
	{
		if (ShipGame::getSingleton().GetRoot() != NULL)
			ShipGame::getSingleton().GetRoot()->removeFrameListener(this);
	}

	virtual void Startup()
	{
		isStarted = true;
	}

	virtual void Shutdown()
	{
		isStarted = false;
	}

	bool frameStarted(const FrameEvent& evt)
	{
		if (this->isStarted)
			return FrameStarted(evt);

		return true;
	}

	bool frameEnded(const FrameEvent& evt)
	{
		if (this->isStarted)			
			return FrameEnded(evt);

		return true;
	}

	virtual bool FrameStarted(const FrameEvent& evt) {return true;}
	virtual bool FrameEnded(const FrameEvent& evt) {return true;}

	bool IsStarted() const {return isStarted;}

protected:
	bool			isStarted;
};

#endif /* PERSISTENTFRAMELISTENER_H_ */
