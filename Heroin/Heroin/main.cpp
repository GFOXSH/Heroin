#include <windows.h>
#include <gl/gl.h>					// OpenGL functions
#include "./msdetours/detours.h"	// Need to replace with custom detours library 
#pragma comment(lib, "Opengl32.lib")

typedef void (APIENTRY* Type_glBegin)(GLenum mode);
typedef void (APIENTRY* Type_glLightf)(GLenum num, GLenum pname, GLfloat param);
Type_glBegin original_glBegin = 0;
Type_glLightf original_glLightf = 0;

void mod_glBegin(GLenum mode) {
	glShadeModel(GL_FLAT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DITHER);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	glDisable(GL_COLOR_MATERIAL);
	(original_glBegin)(mode);
}

void mod_glLightf(GLenum num) {
	(original_glLightf)(num, GL_SPOT_CUTOFF, 180.0);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	DisableThreadLibraryCalls((HMODULE)hinstDLL);
	if (fdwReason == DLL_PROCESS_ATTACH) {
		original_glBegin = (Type_glBegin)(DetourFunction((unsigned char*)GetProcAddress(LoadLibrary("opengl32.dll"), "glBegin"), (unsigned char*)mod_glBegin));
		original_glLightf = (Type_glLightf)(DetourFunction((unsigned char*)GetProcAddress(LoadLibrary("opengl32.dll"), "glLightf"), (unsigned char*)mod_glLightf));
	}
	else if (fdwReason == DLL_PROCESS_DETACH) {
		original_glBegin = (Type_glBegin)(DetourFunction((unsigned char*)GetProcAddress(LoadLibrary("opengl32.dll"), "glBegin"), (unsigned char*)original_glBegin));
		original_glLightf = (Type_glLightf)(DetourFunction((unsigned char*)GetProcAddress(LoadLibrary("opengl32.dll"), "glLightf"), (unsigned char*)original_glLightf));
	}
	return true;
}
