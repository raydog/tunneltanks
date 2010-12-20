#!/bin/sh

test_program() {
	which "$1" 1>/dev/null 2>/dev/null
	if [ $? -ne 0 ] ; then
		echo "'$1' is not in the currently visible. Are you sure that"
		echo "$2 is in the PATH?"
		exit 1
	fi
}

print_message() {
	echo ""
	echo "======================================="
	echo "$1"
	echo "======================================="
}

test_program "ndk-build" "the Android NDK"
test_program "android"   "the Android SDK's tools directory"

TARGET="debug"
if [ "$1" = "--release" ] ; then
	TARGET="release"
fi

print_message "COMPILING THE C PROJECT..."
ndk-build
if [ "$?" -ne 0 ] ; then
	echo ""
	echo "Failed to compile. :("
	exit 1
fi

print_message "UPDATING PROJECT CONFIGURATION..."
android update project -p . 
if [ "$?" -ne 0 ] ; then
	echo ""
	echo "Failed to configure. :("
	exit 1
fi

print_message "COMPILING JAVA PROJECT..."
ant -q $TARGET
if [ "$?" -ne 0 ] ; then
	echo ""
	echo "Failed to compile. :("
	exit 1
fi

