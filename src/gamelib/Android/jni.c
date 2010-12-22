#include <jni.h>

#include <game.h>
#include <gamelib.h>

#include "androiddata.h"
#include "require_android.h"


static void handle_dir(JNIEnv *env, jclass classy, jint x, jint y) {
	_DATA.c_x = x;
	_DATA.c_y = y;
}

static void handle_shoot(JNIEnv *env, jclass classy, jint is_shooting) {
	_DATA.c_shoot = !!is_shooting;
}

static jint handle_step(JNIEnv *env, jclass classy, jclass bitmap) {
	return game_step(_DATA.gd);
}


#define _METHOD(name, sig, ptr) ((JNINativeMethod){(name), (sig), (ptr)})

/* When the library is first loaded, connect, and init everything: */
jint JNI_OnLoad(JavaVM *vm, void *ignored) {
	JNIEnv *env;
	jclass  classy;
	
	JNINativeMethod list[] = {
		_METHOD("setControllerDirection", "(II)V",          handle_dir),
		_METHOD("setControllerShooting", "(I)V",            handle_shoot),
		_METHOD("gameStep", "(Landroid/graphics/Bitmap;)I", handle_step)
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

