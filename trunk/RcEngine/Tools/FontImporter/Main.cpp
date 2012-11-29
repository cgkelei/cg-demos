#include "Common.h"
#include "Font.h"
#include "nvImage/include/nvImage.h"

#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "nvImaged.lib")
#pragma comment(lib, "opengl32.lib")

using namespace std;
using namespace DistanceField;

Font* gFont;

//wstring gUnicodeString =L"\t���ļ���ʽ��\n\
//						 ������ȷ�ͱ�Ϊδ֪\n\
//						 �����ߣ�	������ȷ�ͱ�Ϊδ֪\n\
//						 ר����		������ȷ�ͱ�Ϊδ֪\n\
//						 ����ʱ�䣺01:01:00����1Сʱ��\n\
//						 09:09����Сʱ��00:09����1����\n\
//						 glBindTexture(GL_TEXTURE_2D,pCharTex->m_texID);\n\
//						 glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );\n\
//						 glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );\n\
//						 glEnable(GL_BLEND);\n\
//						 glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);" ;

wstring gUnicodeString =L"֠";

GLuint gTexture;
GLuint gProgram;
GLint  gTexParam;

void init(void)
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glEnable ( GL_COLOR_MATERIAL );
	glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
	glEnable ( GL_TEXTURE_2D );
	glDisable ( GL_CULL_FACE );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	/*glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);*/

	int status;
	bool valid;

	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	std::ifstream vif("distance-field.vert");
	std::string vStr((std::istreambuf_iterator<char>(vif)), std::istreambuf_iterator<char>());
	const char* vs =  vStr.c_str();
	glShaderSource(vertexShaderID, 1, &vs, NULL);
	glCompileShader(vertexShaderID);	
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &status);
	valid = (status != 0);

	if (!valid)
	{
		string err;
		int length;
		glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &length);
		err.resize(length);
		glGetShaderInfoLog(vertexShaderID, length, &length, &err[0]);
		printf("%s\n", err.c_str());
	}


	GLuint pixelShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	std::ifstream fif("distance-field.frag");
	std::string fStr((std::istreambuf_iterator<char>(fif)), std::istreambuf_iterator<char>());
	const char* fs =  fStr.c_str();
	glShaderSource(pixelShaderID, 1, &fs, NULL);
	glCompileShader(pixelShaderID);
	glGetShaderiv(pixelShaderID, GL_COMPILE_STATUS, &status);
	valid = (status != 0);

	if (!valid)
	{
		string err;
		int length;
		glGetShaderiv(pixelShaderID, GL_INFO_LOG_LENGTH, &length);
		err.resize(length);
		glGetShaderInfoLog(pixelShaderID, length, &length, &err[0]);
		printf("%s\n", err.c_str());
	}

	gProgram = glCreateProgram();
	glAttachShader(gProgram, vertexShaderID);
	glAttachShader(gProgram, pixelShaderID);
	glLinkProgram(gProgram);

	glGetProgramiv(gProgram, GL_LINK_STATUS, &status);
	valid = (status != 0);

	if (!valid)
	{
		string err;
		int length;
		glGetProgramiv(gProgram, GL_INFO_LOG_LENGTH, &length);
		err.resize(length);
		glGetProgramInfoLog(gProgram, length, &length, &err[0]);
		printf("%s\n", err.c_str());
	}
	
	gTexParam = glGetUniformLocation(gProgram, "texture");

	gFont = new Font("msyh.ttf", 32, gUnicodeString);

	glUniform1i(gTexParam, gFont->mTextureID);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	// Create font texture
	/*glGenTextures(1, &gTexture);
	glBindTexture(GL_TEXTURE_2D,gTexture);

	nv::Image image;
	image.loadImageFromFile("DistanceFieldAum.png");

	glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, image.getWidth(), image.getHeight(), 0,
		GL_ALPHA,GL_UNSIGNED_BYTE,image.getLevel(0));

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );*/
}

void reshape( int w, int h )
{
	// Reset the coordinate system before modifying

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
	glOrtho(0,w,h,0, -1, 1);
	// Set the clipping volume
	glMatrixMode(GL_MODELVIEW);
}

void display( void )
{
	glClearColor(0.0f , 0.0f , 0.0f , 1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();

	glUseProgram(gProgram);

	/*glUseProgram(0);
	glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);*/

	//glBindTexture(GL_TEXTURE_2D, gTexture);
	//glActiveTexture(GL_TEXTURE0);

	//glColor3f(1, 0, 0);
	//glBegin(GL_QUADS);
	//	glTexCoord2f(0, 0); glVertex3f(100, 100,  1.0f);

	//	// top right
	//	glTexCoord2f(1, 0); glVertex3f(400, 100    ,  1.0f);

	//	// bottom right
	//	glTexCoord2f(1, 1);  glVertex3f(400 , 400,  1.0f);

	//	// bottom left
	//	glTexCoord2f(1, 0); glVertex3f(100, 400,  1.0f);
	//glEnd();

	//gFont->DrawText(gUnicodeString, 10, 50, 64);
	gFont->DrawText(gUnicodeString, 100, 300, 256);

	/*gFont->DrawText(L"�������123456789", 10, 100, 64);
	gFont->DrawText(L"����ֻ˵���������֡���Ҫ��ҵ���Ǳ�˵��ҵ�ܼ��ʧ���ʺܸߣ���ɹ�������", 10, 150, 32);
	gFont->DrawText(L"�����������£���Ը������Ӳ�ң����������룬Ը���ܳ���", 10, 200, 32);
	gFont->DrawText(L"��֧��ͨ���������ɹ�����#С���ֻ�1S�ഺ��#���û��Ѿ���", 10, 250, 32);
	gFont->DrawText(L"�Ї��L����Y����������", 10, 300, 32);*/

	glutSwapBuffers();
}

void keyboard ( unsigned char key, int x, int y )  // Create Keyboard Function
{
	switch ( key ) {
	case 27:        // When Escape Is Pressed...
		exit ( 0 );   // Exit The Program
		break;        // Ready For Next Case
	default:        // Now Wrap It Up
		break;
	}
}

void arrow_keys ( int a_keys, int x, int y )  // Create Special Function (required for arrow keys)
{
	switch ( a_keys ) {
	case GLUT_KEY_UP:     // When Up Arrow Is Pressed...
		glutFullScreen ( ); // Go Into Full Screen Mode
		break;
	case GLUT_KEY_DOWN:               // When Down Arrow Is Pressed...
		glutReshapeWindow ( 800, 800 ); // Go Into A 500 By 500 Window
		break;
	default:
		break;
	}
}

int main ( int argc, char** argv )   // Create Main Function For Bringing It All Together
{
	glutInit            ( &argc, argv ); // Erm Just Write It =)
	glutInitDisplayMode ( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA ); // Display Mode
	glutInitWindowPosition (0,0);
	glutInitWindowSize  ( 500, 500 ); // If glutFullScreen wasn't called this is the window size
	glutCreateWindow    ( "NeHe Lesson 6- Ported by Rustad" ); // Window Title (argv[0] for current directory as title)
	
	glewInit();
	
	init ();
	//glutFullScreen      ( );          // Put Into Full Screen
	glutDisplayFunc     ( display );  // Matching Earlier Functions To Their Counterparts
	glutReshapeFunc     ( reshape );
	glutKeyboardFunc    ( keyboard );
	glutSpecialFunc     ( arrow_keys );
	glutIdleFunc			 ( display );
	glutMainLoop        ( );          // Initialize The Main Loop
	return 1;
}


