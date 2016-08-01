#include "PdfReader.h"
#include "imgui/imgui.h"
#include "platform.h"
#include <GL/gl3w.h>
#include <thread>
#include <iostream>

#define SCALING 2

void ButtonRect(const char *text, ImVec4 coord) {
	ImGui::SetCursorPosX(coord.x*SCALING);
	ImGui::SetCursorPosY(coord.y*SCALING);
	ImGui::Button(text, ImVec2(coord.z*SCALING, coord.w*SCALING));
}

static inline bool isResultValid(ImVec4 vec) {
	return vec.x || vec.y || vec.z || vec.w;
}

ImVec4 PdfReader::search(std::string text, poppler::page *page) {
	//poppler::rectangle<double> rect(0,0,0,0);
	bool found = page->search(poppler::ustring::from_latin1(text), searchRectangle, poppler::page::search_next_result, poppler::case_insensitive, poppler::rotate_0);

	if (found)
		return ImVec4(searchRectangle.x(), searchRectangle.y(), searchRectangle.width(), searchRectangle.height());
	else
		return ImVec4(0, 0, 0, 0);
}

ImTextureID PdfReader::surfaceToTexture(SDL_Surface *surface) {
	GLuint textureid;
	glGenTextures(1, &textureid);
	glBindTexture(GL_TEXTURE_2D, textureid);
	glTexImage2D(GL_TEXTURE_2D, 0,  GL_COMPRESSED_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	SDL_FreeSurface(surface);
	return (ImTextureID)textureid;
}

RenderedPage PdfReader::renderPage(poppler::page *page) {
	poppler::image image = prend->render_page(page, 72.0*SCALING, 72.0*SCALING);

	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(image.data(), image.width(), image.height(), 32, image.bytes_per_row(), 0, 0, 0, 0);

	return RenderedPage(image.width(), image.height(), surface);
}

void PdfReader::renderPages() {
	std::string netName = "VSS";

	for (int i = 0; i < 30; i++) {
		poppler::page *page = document->create_page(i);
		if (!page) {
			printf("Could not open page %d of document\n", i);
			continue;
		}

		renderedPages[i] = renderPage(page);

		ImVec4 result;
		do {
			result = search(netName, page);
//			printf("%f %f %f %f\n", result.x, result.y, result.z, result.w);
			if(isResultValid(result)) {
				NetButton netButton(netName, i, result.x, result.y, result.z, result.w);
				netButtons.push_back(netButton);
			}
		} while (isResultValid(result));
		delete page;

		searchRectangle = {0.0, 0.0, 0.0, 0.0};
		renderedPages[i].valid.store(true);
	}
}

PdfReader::PdfReader() {
	document = poppler::document::load_from_file(filename);
	if (!document) {
		printf("Error while loading: %s\n", filename);
		return;
	}

	prend = new poppler::page_renderer();
	prend->set_render_hints(poppler::page_renderer::antialiasing | poppler::page_renderer::text_antialiasing | poppler::page_renderer::text_hinting);

	std::thread(&PdfReader::renderPages, this).detach();
}

PdfReader::~PdfReader() {
	delete prend;
}

void PdfReader::Draw(const char *title, bool *p_open, Board *board) {
	int width  = 1024;
	int height = 640;

	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::Begin("Pdf Reader", NULL, ImGuiWindowFlags_HorizontalScrollbar);

	for (auto &renderedPage: renderedPages) {
//		std::cout << "Page number: " << renderedPage.first << ", valid: " << renderedPage.second.valid.load() << std::endl;
		if (!renderedPage.second.valid.load()) continue;
		if (renderedPage.second.surface != nullptr)
		{
			renderedPage.second.textureId = surfaceToTexture(renderedPage.second.surface);
			renderedPage.second.surface = nullptr;
		}
		if (renderedPage.second.textureId == nullptr)
			continue;
		renderedPage.second.offset = ImGui::GetCursorPos();
		ImGui::Image(renderedPage.second.textureId, ImVec2(renderedPage.second.width, renderedPage.second.height));
		ImGui::Separator();
	}

	for (auto &netButton: netButtons) {
//		std::cout << "Button x: " << netButton.x << " y: " << netButton.y << std::endl;
		RenderedPage renderedPage = renderedPages[netButton.pageNum];
		bool valid = renderedPage.valid.load();
		std::cout << "page_num: " << netButton.pageNum << ", valid: " << valid << std::endl;
//		if (!renderedPage.valid.load()) continue;
		ImVec2 offset = renderedPage.offset;
		ImVec4 but(netButton.x+(offset.x/SCALING), netButton.y+(offset.y/SCALING), netButton.z, netButton.w);
		ButtonRect("", but);
	}

	ImGui::End();
}
