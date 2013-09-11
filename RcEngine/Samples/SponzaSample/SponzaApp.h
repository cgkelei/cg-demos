#pragma once
#ifndef SponzaApp_h__
#define SponzaApp_h__

#include <MainApp/Application.h>

using namespace RcEngine;

class SponzaApp : public Application
{
public:
	SponzaApp(const String& config);
	~SponzaApp(void);

protected:

	void Initialize();
	void LoadContent();
	void UnloadContent();
	void Render();
	void Update(float deltaTime);

protected:


};

#endif // SponzaApp_h__
