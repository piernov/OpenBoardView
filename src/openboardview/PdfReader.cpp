#include "PdfReader.h"
#include "imgui/imgui.h"
#include "platform.h"
#include <GL/gl3w.h>
#include <thread>
#include <iostream>
#include <memory>
#include <locale>
#include <codecvt>

#define SCALING 2

void ButtonRect(const char *text, ImVec4 coord) {
	ImGui::SetCursorPosX(coord.x*SCALING);
	ImGui::SetCursorPosY(coord.y*SCALING);
	ImGui::Button(text, ImVec2(coord.z*SCALING, coord.w*SCALING));
}

static inline bool isResultValid(ImVec4 vec) {
	return vec.x || vec.y || vec.z || vec.w;
}

void PdfReader::search(std::string text, FPDF_PAGE page, int pageNum, std::vector<SearchResult> &results) { // ASCII only for now (using string::size)
	double left, top, right, bottom;
	int rotation = FPDFPage_GetRotation(page);

	FPDF_TEXTPAGE textpage = FPDFText_LoadPage(page);
	std::u16string utf16 = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(text.data());
	FPDF_WIDESTRING fpdf_wstr = reinterpret_cast<const std::uint_least16_t*>(utf16.c_str());

	FPDF_SCHHANDLE schhandle = FPDFText_FindStart(textpage, fpdf_wstr, FPDF_MATCHWHOLEWORD, 0);
//	bool found = FPDFText_FindNext(schhandle);
	while (FPDFText_FindNext(schhandle)) {
		int index = FPDFText_GetSchResultIndex(schhandle);
		int count = FPDFText_CountRects(textpage, index, text.size());
		FPDFText_GetRect(textpage, 0, &left, &top, &right, &bottom);

		switch(rotation) {
			case 0:
				FPDFText_GetRect(textpage, 0, &left, &bottom, &right, &top);
				break;
			case 1:
				FPDFText_GetRect(textpage, 0, &bottom, &left, &top, &right);
				break;
		}
		printf("Name: %s, Index: %d, count: %d, left: %f, top: %f, right: %f, bottom: %f\n", text.c_str(), index, count, left, top, right, bottom);


//		SearchResult searchResult(text, pageNum, bottom, left, top-bottom, right-left);
		SearchResult searchResult(text, pageNum, left, top, right-left, top-bottom);

		mutex.lock();
		results.push_back(searchResult);
		mutex.unlock();
	}
	FPDFText_FindClose(schhandle);
	FPDFText_ClosePage(textpage);

}

ImTextureID PdfReader::bitmapToTexture(FPDF_BITMAP bitmap, int width, int height) {
	GLuint textureid;
	glGenTextures(1, &textureid);
	glBindTexture(GL_TEXTURE_2D, textureid);
	glTexImage2D(GL_TEXTURE_2D, 0,  GL_COMPRESSED_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, FPDFBitmap_GetBuffer(bitmap));
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	FPDFBitmap_Destroy(bitmap);
	return reinterpret_cast<ImTextureID>(textureid);
}

std::unique_ptr<RenderedPage> PdfReader::renderPage(FPDF_PAGE page) {
	int width = static_cast<int>(FPDF_GetPageWidth(page) * SCALING);
	int height = static_cast<int>(FPDF_GetPageHeight(page) * SCALING);
	FPDF_BITMAP bitmap = FPDFBitmap_Create(width, height, 1);
	FPDFBitmap_FillRect(bitmap, 0, 0, width, height, 0xFFFFFFFF);
	FPDF_RenderPageBitmap(bitmap, page, 0, 0, width, height, 0, 0);
	std::unique_ptr<RenderedPage> renderedPage(new RenderedPage(bitmap, width, height));
/*	renderedPage->bitmap = bitmap;
	renderedPage->height = height;
	renderedPage->width = width;
*/
	return renderedPage;
}

void PdfReader::renderPages() {
//	std::string netName = "Processor";

//	int page_count = FPDF_GetPageCount(document);
	int page_count = 10;

	for (int i = 0; i < page_count; i++) {
		FPDF_PAGE page = FPDF_LoadPage(document, i);
		if (!page) {
			printf("Could not open page %d of document\n", i);
			continue;
		}

		std::unique_ptr<RenderedPage> renderedPage = renderPage(page);
		mutex.lock();
		renderedPages.push_back(std::move(renderedPage));
		mutex.unlock();

	}
	if (!board) return;
	auto nets = board->Nets();
	for (int i = 0; i < page_count; i++) {
		FPDF_PAGE page = FPDF_LoadPage(document, i);
		if (!page) {
			printf("Could not open page %d of document\n", i);
			continue;
		}
		for(auto &net: nets) {
//			std::cout << net->name << std::endl;
			search(net->name, page, i, netButtons);
		}

		FPDF_ClosePage(page);
	}
}

PdfReader::PdfReader(Board *board): board(board) {
	FPDF_InitLibrary();

	document = FPDF_LoadDocument(filename, NULL);
	if (document == NULL) {
		std::cerr << "Error while loading " << filename << std::endl;
		return;
	}

/*	FPDF_PAGE page = FPDF_LoadPage(document, 0);
	if (page == NULL) {
		std::cerr << "Error while loading page" << 0 << std::endl;
		return;
	}*/

	std::thread(&PdfReader::renderPages, this).detach();
}

PdfReader::~PdfReader() {
	FPDF_CloseDocument(document);
	FPDF_DestroyLibrary();
}

void PdfReader::Draw(const char *title, bool *p_open, Board *board) {
	int width  = 1024;
	int height = 640;

//	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::Begin("Pdf Reader", NULL, ImGuiWindowFlags_HorizontalScrollbar);

	if(mutex.try_lock()) {
		for (auto &renderedPage: renderedPages) {
//		if (!renderedPage->valid.load()) continue;
			int width = renderedPage->width;
			int height = renderedPage->height;
			if (renderedPage->bitmap != nullptr)
			{
				renderedPage->textureId = bitmapToTexture(renderedPage->bitmap, width, height);
				renderedPage->bitmap = nullptr;
			}
			if (renderedPage->textureId == nullptr)
				continue;


			renderedPage->offset = ImGui::GetCursorPos();
			ImGui::Image(renderedPage->textureId, ImVec2(width, height));
			ImGui::Separator();
		}
//	}
//	mutex.unlock();


	for (auto &netButton: netButtons) {
//		std::unique_ptr<RenderedPage> renderedPage = renderedPages.at(netButton.pageNum);
//		bool valid = renderedPage->valid.load();
//		std::cout << "page_num: " << netButton.pageNum << std::endl;
//		if (!renderedPage.valid.load()) continue;
		ImVec2 offset = renderedPages.at(netButton.pageNum)->offset;
		ImVec4 but(netButton.x+(offset.x/SCALING), renderedPages.at(netButton.pageNum)->height - netButton.y + (offset.y/SCALING), netButton.z, /*-*/netButton.w);
		printf("x: %f, y: %f, w: %f, h: %f\n", but.x, but.y, but.z, but.w);
		ButtonRect("", but);
	}

mutex.unlock();
}

	ImGui::End();
}
