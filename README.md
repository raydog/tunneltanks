#TunnelTanks

This project is a slightly modernized version of a 1991 DOS game called Tunneler, by Geoffrey Silverton. This port is powered by SDL, and licensed under the GPLv3.

For a little bit of context, check out the 'tunneler' tag on my blog: http://blog.poweredbytoast.com/tag/tunneler

##Building

This thing needs CMake to configure, a Make-like program to compile, and the SDL
libraries to run, so make sure you have all of those...

To test this program out, cd into this directory, and run:

```bash
cmake . && make && ./tunneltanks
```

To install, run:

```bash
sudo make install
```

To return the project to it's pristine starting state, run:

```bash
make dist-clean
```

That command may need to be run as root if you did a make install, since make
install also needs to be run as root and may leave a root-owned file in the
directory. You can blame CMake for this inconvenience...

Also, this project has some packaging targets, so try:

* `make package-tbz2` to make a binary tar.bz2,
* `make package-deb` to make a debian package file, and
* `make package-rpm` to make a RPM package file. (Requires rpmbuild.)

##Android Build
*(Experimental, and probably out-of-date...)*

If you wish to compile the Android port of this project, then first make sure
that you have both the Android SDK and NDK downloaded, extracted into 
directories, and that the SDK's root directory and the NDK's "tools" directory
are both in the current $PATH. Then, cd into the android subdirectory and run
the build.sh script. That will compile everything needed, and assemble the
resulting debug apk's in the bin subdirectory.