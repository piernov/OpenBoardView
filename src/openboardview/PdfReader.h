#pragma once

#include <atomic>
#include <SDL2/SDL.h>
#include "BRDFile.h"
#include "Board.h"
#include "imgui/imgui.h"
#include <mutex>

#include <fpdfview.h>
#include <fpdf_edit.h>
#include <fpdf_text.h>

struct RenderedPage {
	int width;
	int height;
	ImVec2 offset;
	ImTextureID textureId = nullptr;
	FPDF_BITMAP bitmap = nullptr;

	RenderedPage(FPDF_BITMAP bitmap, int width, int height): width(width), height(height), bitmap(bitmap) {}
//	std::atomic<bool> valid;

/*	friend void swap(RenderedPage& f, RenderedPage& s)
	{
		using std::swap;
		swap(f.width, s.width);
		swap(f.height, s.height);
		swap(f.offset, s.offset);
		swap(f.textureId, s.textureId);
		swap(f.bitmap, s.bitmap);
		f.valid.store(f.valid.exchange(s.valid.load()));
	}
*/
/*	RenderedPage(FPDF_BITMAP bitmap, int width, int height): width(width), height(height), bitmap(bitmap) {
		valid.store(false);
	}
*/
/*
	RenderedPage() {
		valid.store(false);
	}

	RenderedPage(const RenderedPage &s): width(s.width), height(s.height), offset(s.offset), textureId(s.textureId), bitmap(s.bitmap) {
		valid.store(s.valid.load());
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
	}*/
};

struct SearchResult: public ImVec4 {
	std::string text;
	int pageNum;
	SearchResult(std::string text, int pageNum, float x, float y, float z, float w): ImVec4(x, y, z, w), text(text), pageNum(pageNum) {}
};

class PdfReader {
public:
	PdfReader(Board *board);
	~PdfReader();

	void Draw(const char *title, bool *p_open, Board *board);
	void search(std::string text, FPDF_PAGE page, int pageNum, std::vector<SearchResult> &results);
	std::unique_ptr<RenderedPage> renderPage(FPDF_PAGE page);
	void renderPages();
	ImTextureID bitmapToTexture(FPDF_BITMAP bitmap, int width, int height);
private:
	FPDF_DOCUMENT document;
	const char *filename = "test.pdf";

	std::vector<std::unique_ptr<RenderedPage>> renderedPages;
	std::vector<SearchResult> netButtons;
	std::mutex mutex;
	Board *board;
};
