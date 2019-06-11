#pragma once

#include "Board.h"

#include <memory>
#include <string.h>
#include <vector>

using namespace std;

class BRDBoard : public Board {
  public:
	BRDBoard(const BRDFile *const boardFile);
	~BRDBoard();

	const BRDFile *m_file;

	EBoardType BoardType();

	std::vector<Net> &Nets();
	std::vector<Component> &Components();
	std::vector<Pin> &Pins();
	std::vector<Point> &OutlinePoints();

  private:
	static const string kNetUnconnectedPrefix;
	static const string kComponentDummyName;

	std::vector<Net> nets_;
	std::vector<Component> components_;
	std::vector<Pin> pins_;
	std::vector<Point> outline_;
};
