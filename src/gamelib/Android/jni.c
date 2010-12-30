#include <stdio.h>
#include <android/bitmap.h>
#include <jni.h>

#include <game.h>
#include <gamelib.h>
#include <types.h>

#include "androiddata.h"
#include "require_android.h"


static void handle_touch(JNIEnv *env, jclass classy, jint is_touching) {
	_DATA.c_is_touching = is_touching;
}

static void handle_move(JNIEnv *env, jclass classy, jint x, jint y) {
	_DATA.c_touch = VECTOR(x, y);
}

static void handle_dir(JNIEnv *env, jclass classy, jint x, jint y) {
	_DATA.c_dir = VECTOR(x, y);
}

static void handle_button(JNIEnv *env, jclass classy, jint is_pressing) {
	_DATA.c_button = !!is_pressing;
}

static jint handle_step(JNIEnv *env, jclass classy, jclass bitmap) {
	AndroidBitmapInfo info;
	
	/* Load up the bitmap: */
	if( AndroidBitmap_getInfo(env, bitmap, &info)<0 ) {
		/* Bitmap load failed! */
		fprintf(stderr, "GetInfo() failed on the provided bitmap.\n");
		return 1; /* Will trigger an exit. */
	}
	
	/* Make sure the bitmap is the correct format: */
	if( info.format != ANDROID_BITMAP_FORMAT_RGB_565 ) {
		fprintf(stderr, "We only understand RGB565 format for now...\n");
		return 1;
	}
	
	_DATA.env    = env;
	_DATA.bitmap = bitmap;
	
	/* Now step the simulation: */
	return game_step(_DATA.gd);
}


#define _METHOD(name, sig, ptr) ((JNINativeMethod){(name), (sig), (ptr)})

/* When the library is first loaded, connect, and init everything: */
jint JNI_OnLoad(JavaVM *vm, void *ignored) {
	JNIEnv *env;
	jclass  classy;
	
	JNINativeMethod list[] = {
		_METHOD("setIsTouching", "(II)V",                        handle_move),
		_METHOD("setTouch",      "(I)V",                         handle_touch),
		_METHOD("setDirection",  "(II)V",                        handle_dir),
		_METHOD("setPress",      "(I)V",                         handle_button),
		_METHOD("gameStep",      "(Landroid/graphics/Bitmap;)I", handle_step)
	};
	
	if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK)
		return -1;
	
	/* Find our native interface class: */
	classy = (*env)->FindClass(env, "TunnelTanksNative");
	
	/* Register all of our native functions: */
	(*env)->RegisterNatives(env, classy, list, 3);
	
	/* Now, init the actual game data, and we're good to go! */
	gamelib_init();
	
	return JNI_VERSION_1_6;
}

/* When the libary is garbage collected, free all of our resources: */
void JNI_OnUnload(JavaVM *vm, void *ignored) {
	gamelib_exit();
}

