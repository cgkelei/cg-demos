#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <string>
#include <sstream>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#define MEDIA_ROOT_PATH "../Media/"
#define MEDIA_SHADER_PATH "../Media/Shaders/"
#define MEDIA_MODEL_PATH "../Media/Models/"
#define MEDIA_TEXTURE_PATH "../Media/Textures/"


#define SAFE_DELETE(p) if(p) { delete p; p = NULL; }
#define ASSERT(condition) { if(!(condition)) { std::cerr << "ASSERT FAILED: " << #condition << " @ " << __FILE__ << " (" << __LINE__ << ")" << std::endl; } assert(condition); }