#include "ImGuiRendererSDLGL1.h"

#include "backends/imgui_impl_opengl2.h"

std::string ImGuiRendererSDLGL1::name() {
    return "ImGuiRendererSDLGL1";
}

bool ImGuiRendererSDLGL1::checkGLVersion() {
	if (GLVersion.major < 1 || (GLVersion.major == 1 && GLVersion.minor < 1)) {
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Minimal OpenGL version required is %d.%d. Got %d.%d.", 1, 1, GLVersion.major, GLVersion.minor);
		return false;
	}
	return true;
}

void ImGuiRendererSDLGL1::setGLVersion() {
	ImGuiRendererSDL::setGLVersion();
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
}

bool ImGuiRendererSDLGL1::init() {
	return ImGuiRendererSDL::init() && ImGui_ImplOpenGL2_Init();
}

void ImGuiRendererSDLGL1::initFrame() {
	ImGui_ImplOpenGL2_NewFrame();
	ImGuiRendererSDL::initFrame();
}

void ImGuiRendererSDLGL1::renderDrawData() {
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiRendererSDLGL1::shutdown() {
	ImGui_ImplOpenGL2_Shutdown();
	ImGuiRendererSDL::shutdown();
}

void *ImGuiRendererSDLGL1::createTexture(uint8_t* data, int w, int h, char fmt) {
	GLuint tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (GLVersion.major == 2 || (GLVersion.major == 1 && GLVersion.minor >= 4)) {
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA_EXT : GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return reinterpret_cast<void*>(tex);
}
