#pragma once

#include <atomic>
#include <SDL2/SDL.h>
#include "BRDFile.h"
#include "Board.h"
#include "imgui/imgui.h"


#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-image.h>
#include <poppler/cpp/poppler-page-renderer.h>

struct RenderedPage {
	int width;
	int height;
	ImVec2 offset;
	ImTextureID textureId = nullptr;
	SDL_Surface *surface = nullptr;
	std::atomic<bool> valid;

	friend void swap(RenderedPage& f, RenderedPage& s)
	{
		using std::swap;
		swap(f.width, s.width);
		swap(f.height, s.height);
		swap(f.offset, s.offset);
		swap(f.textureId, s.textureId);
		swap(f.surface, s.surface);
		s.valid.store(false);
		f.valid.store(false);
	}

	RenderedPage(int width, int height, SDL_Surface *surface): width(width), height(height), surface(surface) {
		valid.store(false);
	}

	RenderedPage() {
		valid.store(false);
	}

	RenderedPage(const RenderedPage &s): width(s.width), height(s.height), offset(s.offset), textureId(s.textureId), surface(s.surface) {
		valid.store(false);
	}

	~RenderedPage() {
		valid.store(false);
	}

	RenderedPage& operator=(RenderedPage other) {
		swap(*this, other);
		return *this;
	}

	RenderedPage(RenderedPage&& other): RenderedPage() {
		swap(*this, other);
	}
};

struct NetButton: public ImVec4 {
	std::string text;
	int pageNum;
	NetButton(std::string text, int pageNum, float x, float y, float z, float w): ImVec4(x, y, z, w), text(text), pageNum(pageNum) {}
};

class PdfReader {
public:
	PdfReader();
	~PdfReader();

	void Draw(const char *title, bool *p_open, Board *board);
	ImVec4 search(std::string text, poppler::page *page);
	RenderedPage renderPage(poppler::page *page);
	void renderPages();
	ImTextureID surfaceToTexture(SDL_Surface *surface);
private:
	poppler::document *document;
	poppler::page_renderer *prend;
	const char *filename = "test.pdf";

	poppler::rectangle<double> searchRectangle;
//	std::map<int, RenderedPage> renderedPages;
	std::vector<RenderedPage> renderedPages;
	std::vector<NetButton> netButtons;
};
