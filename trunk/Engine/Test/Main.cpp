#include "Application.h"
#include "Vector.h"
#include  "Matrix.h"
#include "RenderSystem.h"
#include "ColorRGBA.h"
#include "RenderWindow.h"

using namespace Engine;


class TestApp : public Application
{
public:
	TestApp() {}

protected:

	 void Initialize()
	 {
		
	 }

	 void Finalize()
	 {

	 }

	
	 void LoadContent() 
	 {
		 
	 }
	
	 void UnloadContent() 
	 {

	 }

	
	 void Render(const GameTime* timer)
	 {

		 mRenderer->UpdateAllRenderTargets(true);

		 mRenderer->Clear(CO_Target, ColorRGBA::White);

		 mWindow->SwapBuffers(false);
	 }

	
	 void Update(const GameTime* timer) 
	 {
		
	 }


};

INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	UNREFERENCED_PARAMETER( hInst );
	TestApp app;
	app.Run();
}
