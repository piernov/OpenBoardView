#include "platform.h"
#include "BoardView.h"
#include <SDL.h>
#include <jni.h>

/*
 * Because we can't call C++ inside a JNI function
 */
void loadFileWrapper(char* path) {
	std::string paths = std::string(path);
	free(path);
	app.LoadFile(paths);
}

extern "C" {
/*
 * Class:     org_openboardview_openboardview_OBVActivity
 * Method:    openFileWrapper
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_openboardview_openboardview_OBVActivity_openFileWrapper
  (JNIEnv * env, jobject o, jstring filePath) {
	const char *utf = env->GetStringUTFChars(filePath, 0);
	if (utf) {
		char *path = SDL_strdup(utf);
		env->ReleaseStringUTFChars(filePath, utf);
		loadFileWrapper(path);
	}
}

}

const filesystem::path show_file_picker(bool filterBoards) {
	JNIEnv *env = (JNIEnv*) SDL_AndroidGetJNIEnv();
	jclass activity = env->FindClass("org/openboardview/openboardview/OBVActivity");
	jmethodID openFilePicker= env->GetStaticMethodID(activity, "openFilePicker", "()V");

	env->CallStaticVoidMethod(activity, openFilePicker);

	env->DeleteLocalRef(activity);
	return {}; // We have to wait for the result, it will call the above JNI function
}

std::vector<char> file_as_buffer(const filesystem::path &filepath, std::string &error_msg) {
	JNIEnv *env = (JNIEnv*) SDL_AndroidGetJNIEnv();
	jclass activity = env->FindClass("org/openboardview/openboardview/OBVActivity");
	jmethodID readFile = env->GetStaticMethodID(activity, "readFile", "(Ljava/lang/String;)[B");

	jstring uris = env->NewStringUTF(filepath.string().c_str());
	jbyteArray jbuffer = reinterpret_cast<jbyteArray>(env->CallStaticObjectMethod(activity, readFile, uris));

	env->DeleteLocalRef(activity);

	// Catch any exception that occured in Java calls to report error message
	if(env->ExceptionCheck()) {
		jthrowable e = env->ExceptionOccurred();
		env->ExceptionDescribe(); // writes to logcat
		env->ExceptionClear();

		jclass clazz = env->GetObjectClass(e);
		jmethodID getMessage = env->GetMethodID(clazz, "getMessage", "()Ljava/lang/String;");
		jstring message = reinterpret_cast<jstring>(env->CallObjectMethod(e, getMessage));
		const char *mstr = env->GetStringUTFChars(message, NULL);
		error_msg = mstr;
		env->ReleaseStringUTFChars(message, mstr);
		env->DeleteLocalRef(message);
		env->DeleteLocalRef(clazz);
		env->DeleteLocalRef(e);

		return {};
	}

	//convert jbyteArray to vector<char>
	jsize len = env->GetArrayLength(jbuffer);
	std::vector<char> fileBuffer(len);
	env->GetByteArrayRegion(jbuffer, 0, len, (jbyte*)fileBuffer.data());

	return fileBuffer;
}

std::string get_asset_path(const char* asset) {
	std::string path = "/sdcard/openboardview";
	path += "/";
	path += asset;
	return path;
}

// Dummy, there is no proper way to search for or enumerate fonts so force Droid Sans
const std::string get_font_path(const std::string &name) {
	return "/system/fonts/DroidSans.ttf";
}

// Don't care about userdir and put everything in the same dir
// Either appplication external storage if available or internal storage
const std::string get_user_dir(const UserDir userdir) {
	std::string path;
	auto extState = SDL_AndroidGetExternalStorageState();
	if (extState & SDL_ANDROID_EXTERNAL_STORAGE_WRITE)
		path = std::string(SDL_AndroidGetExternalStoragePath());
	else
		path = std::string(SDL_AndroidGetInternalStoragePath());
	if (create_dirs(path))
		return path + "/";
	else
		return "./";
}
