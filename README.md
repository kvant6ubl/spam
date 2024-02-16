## spam

The SPAM (Subtractive Pixel Adjacency Matrix) model is based on the analysis of changes in noise components of a digital image (container) when embedding messages using theory of Markov processes.

The matrix of transition probabilities obtained in the process of modeling inter-pixel dependencies as a higher-order Markov chain is used as a feature vector for the stegoanalyzer. For more references please see corresponding PDF in ``docs/`` folder.

This repository contains the updated version of SPAM extractor of 1st and 2nd order SPAM features from grayscale .png images. This tool is based on related works by T. Pevny, P. Bas and J. Fridrich.

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


### Usage
To use this feature extractor you need to allocate the input directory to greyscale PNG images (with ``inputDir`` argument), as well as specify upper bound on absolute value of differences on 1st order SPAM features (with ``T1`` flag) and 2nd order SPAM features (with ``T2`` flag):

```
usage: spam inputDir [-T1] [-T2] [--oFile1st] [--oFile2nd]
```

For example, to get 1st order SPAM features based on ``img_cover`` as input directory to greyscale PNG images call:

```sh
$ ./spam img_cover/ --oFile1st features/img_cover_features_1st.txt 
```

The output would be:
```
7.0548651550e-02 8.5344252452e-02 8.6745859358e-02 7.9413126534e-02 ... 00001.png
3.1352702929e-02 5.0772029679e-02 7.6292191638e-02 1.0801989712e-01 ... 00002.png
8.4047945956e-02 9.1410392438e-02 9.4881927207e-02 8.8133303397e-02 ... 00003.png
4.0026460292e-02 5.4879773949e-02 7.2883518260e-02 9.2701054888e-02 ... 00004.png
5.9828274841e-02 7.2894042238e-02 8.3003773491e-02 9.5885689419e-02 ... 00005.png

```

As was in the previous example, to get 1st order SPAM features based on ``img_stego`` as input directory to greyscale PNG images call:

```sh
$ ./spam img_stego/ --oFile1st features/img_stego_features_1st.txt 
```