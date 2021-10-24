#ifndef _IMGUIRENDERERSDLGL3_H_
#define _IMGUIRENDERERSDLGL3_H_

#include "ImGuiRendererSDL.h"

class ImGuiRendererSDLGL3: public ImGuiRendererSDL {
	using ImGuiRendererSDL::ImGuiRendererSDL;
public:
	std::string name();
	bool checkGLVersion();
	void setGLVersion();
	bool init();
	void initFrame();
	void renderDrawData();
	void shutdown();
	void *createTexture(uint8_t* data, int w, int h, char fmt);
private:
	std::string glsl_version;
};

#endif
