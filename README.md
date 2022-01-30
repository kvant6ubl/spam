## spam

Bug fixed version of extractor of 1st and 2nd order SPAM features from grayscale .png images. This tool is based on related works by T. Pevny, P. Bas and J. Fridrich.

### Fixed issues
1. Fix deprecated ``leaf()`` (``boost`` C++ lib) to get filename of current directory.
2. Changing the ``bit_depth`` field in ``libpng`` lib that holds the bit depth of one of the image channels.
3. Change assigning ``TSpam12`` class members with calling ``libpng's`` function ``png_get_image_width()``, ``png_get_image_height()``, e.g.


### Linux compilation instructions
1. Install boost library:

```sh
$ sudo apt-get install libboost-all-dev
```
2. And just simply build project with ``make``.

### Windows compilation instructions

1. Download and compile ``boost`` from ``boost.org``. Follow instructions on the boost website (getting started) to compile. In commandline run ``bootstrap.bat``, and then run ``./bjam``.
2. Download and compile ``libpng``, which also depends on ``zlib``. The ``libpng`` solution file also includes ``zlib`` project, so you just need to download ``zlib`` and place it in the directory structure explained below. The ``libpng`` readme also mentions this.
3. Put the compiled libraries in the following directory structure (note that the names of the directories have been changed to remove the versioning):

```
./THIRDPARTY/Boost/
./THIRDPARTY/zlib/
./THIRDPARTY/libpng/
```
Open the ```SPAM.sln``` in visual studio and compile. Note that it has been configured only for win32 Debug and Release configurations.