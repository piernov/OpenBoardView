#include "BRDBoard.h"

enum class SearchMode {
	Sub,
	Prefix,
	Whole,
};

class Searcher {
	SearchMode m_searchMode = SearchMode::Sub;

	const std::vector<Net> *m_nets;
	const std::vector<Component> *m_parts;

	template<class T> std::vector<const T*> searchFor(const std::string& search, const std::vector<T> &v,  int limit);
	bool strstrModeSearch(const std::string &strhaystack, const std::string &strneedle);
public:
	void setNets(const std::vector<Net> &nets);
	void setParts(const std::vector<Component> &components);

	bool isMode(SearchMode sm);
	void setMode(SearchMode sm);
	std::vector<const Component*> parts(const std::string& search, int limit);
	std::vector<const Component*> parts(const std::string& search);
	std::vector<const Net*> nets(const std::string& search, int limit);
	std::vector<const Net*> nets(const std::string& search);
};
