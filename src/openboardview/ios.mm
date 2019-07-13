#ifdef __APPLE__

#include "platform.h"
#include <string>
#import <Foundation/Foundation.h>


const std::string show_file_picker() {
	return std::string();
}

const std::string get_font_path(const std::string &name) {
	return std::string();
}

// Inspired by https://developer.apple.com/library/mac/documentation/FileManagement/Conceptual/FileSystemProgrammingGuide/ManagingFIlesandDirectories/ManagingFIlesandDirectories.html
// userdir is ignored for now since common usage puts both config file and history file in ApplicationSupport directory
const std::string get_user_dir(const UserDir userdir) {
	return std::string();
}

void MyNSLog(const char *message)
{
    NSLog(@"%s", message);
}

#endif
