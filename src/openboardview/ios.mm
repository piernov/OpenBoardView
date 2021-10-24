#ifdef __APPLE__

#include "platform.h"
#include <string>
#include "ImFileDialog.h"
#import <Foundation/Foundation.h>

const filesystem::path show_file_picker(bool filterBoards) {
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *documentsDirectory = [paths objectAtIndex:0];
	std::string path = std::string([documentsDirectory UTF8String]);
	ifd::FileDialog::Instance().Open("FileOpenDialog", "Open a boardview file", ".*", false, path);
	return {};
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
