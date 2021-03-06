# LaBGen

[![OpenCV Version](https://img.shields.io/badge/opencv-2.x%2C%203.x-blue.svg)](http://opencv.org) [![Build Status](https://travis-ci.org/benlaug/labgen.svg?branch=master)](https://travis-ci.org/benlaug/labgen)

LaBGen is a patch-based stationary background generation method that was introduced in [[2](#references)] and extensively described in [[1](#references)]. The purpose of this repository is twofold:

1. To share the source code of the method.
2. To embed the method in a ready-to-use program.

![Graphical Abstract](.readme/graphical-abstract.png)

Our method won an award and has been ranked first during the [IEEE Scene Background Modeling Contest (SBMC) 2016](http://pione.dinf.usherbrooke.ca/sbmc2016), and on the [SBI dataset](http://sbmi2015.na.icar.cnr.it/SBIdataset.html) [[4](#references)] according to [three metrics over six](http://sbmi2015.na.icar.cnr.it/SBIdataset.html#[Results]). The current ranking and the quantitative metrics computed on the [SBMnet dataset](http://www.scenebackgroundmodeling.net) are available [here](http://pione.dinf.usherbrooke.ca/results).

![Award](.readme/award.jpg)

Here is a video showing some backgrounds estimated by LaBGen (click on the image below to play it):

[![Demonstration of LaBGen](https://img.youtube.com/vi/rYhX8ZizSL0/0.jpg)](https://www.youtube.com/watch?v=rYhX8ZizSL0 "Click to play")

## Compiling the program

The program implementing the method has been developed in C++11 and is distributed under the [GPLv3](LICENSE) license. In order to compile it, you need a modern C++ compiler, a copy of the [Boost](http://www.boost.org) library, a copy of the [OpenCV](http://opencv.org) library, and the [CMake](https://cmake.org) build automation tool. On UNIX-like environments, the program can be compiled as follows, considering that your terminal is in the source code directory:

```
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make
```

## Running the program

Once the program has been compiled, the following command gives the complete list of available options:

```
$ ./LaBGen-cli --help
```

As an example, the IBMtest2 sequence of the [SBI dataset](http://sbmi2015.na.icar.cnr.it/SBIdataset.html) [[4](#references)] can be processed with the default set of parameters as follows:

```
$ ./LaBGen-cli -i path_to_IBMtest2/IBMtest2_%6d.png -o my_output_path -d -v
```

![Screenshot](.readme/screenshot.png)

A full documentation of the options of the program is [available on the wiki](https://github.com/benlaug/labgen/wiki/Arguments-of-the-program).

## Citation

If you use LaBGen in your work, please cite papers [[1](#references)] and [[2](#references)] as below:

```
@article{Laugraud2017LaBGen,
  title = {{LaBGen}: A method based on motion detection for generating the background of a scene},
  author = {B. Laugraud and S. Pi{\'e}rard and M. {Van Droogenbroeck}},
  journal = {Pattern Recognition Letters},
  publisher = {Elsevier},
  volume = {96},
  pages = {12-21},
  year = {2017},
  doi = {10.1016/j.patrec.2016.11.022}
}

@inproceedings{Laugraud2015Simple,
  title = {Simple median-based method for stationary background generation using background subtraction algorithms},
  author = {B. Laugraud and S. Pi{\'e}rard and M. Braham and M. {Van Droogenbroeck}},
  booktitle = {International Conference on Image Analysis and Processing (ICIAP), Workshop on Scene Background Modeling and Initialization (SBMI)},
  publisher = {Springer},
  series = {Lecture Notes in Computer Science},
  volume = {9281},
  pages = {477-484},
  year = {2015},
  month = {September},
  address = {Genova, Italy},
  doi = {10.1007/978-3-319-23222-5_58}
}
```

## Alternatives

* A variant, called [LaBGen-OF](https://github.com/benlaug/labgen-of), and leverages optical flow algorithms for motion detection.
* A pixel-level variant of LaBGen, called [LaBGen-P](https://github.com/benlaug/labgen-p).

## Testing

Each commited revision is automatically tested using [Travis CI](https://travis-ci.org/benlaug/labgen) on:

* Ubuntu 14.04 with the `g++` compiler and OpenCV 2.3 installed from the Ubuntu repositories.
* Ubuntu 14.04 with the `g++` compiler and OpenCV 3.2 compiled from the sources.
* OS X El Capitan with the `clang++` compiler and OpenCV 2.4 installed with [Homebrew](https://brew.sh).
* OS X El Capitan with the `clang++` compiler and OpenCV 3.2 installed with [Homebrew](https://brew.sh).

## Acknowledgments

This program incorporates some parts of the [BGSLibrary](https://github.com/andrewssobral/bgslibrary) [[3](#references)]. We are very grateful to Andrews Sobral for sharing his library.

## References

[1] [B. Laugraud, S. Piérard, and M. Van Droogenbroeck. LaBGen: A method based on motion detection for generating the background of a scene. *Pattern Recognition Letters*, 96:12-21, 2017.](http://hdl.handle.net/2268/203572)

[2] [B. Laugraud, S. Piérard, M. Braham, M. Van Droogenbroeck. Simple median-based method for stationary background generation using background subtraction algorithms. *International Conference on Image Analysis and Processing (ICIAP), Workshop on Scene Background Modeling and Initialization (SBMI)*, 9281:477-484, 2015.](http://hdl.handle.net/2268/182893)

[3] A. Sobral. BGSLibrary: An OpenCV C++ Background Subtraction Library. *Workshop de Visao Computacional (WVC)*, 2013.

[4] L. Maddalena, A. Petrosino. Towards Benchmarking Scene Background Initialization. *International Conference on Image Analysis and Processing Workshops (ICIAP Workshops)*, 9281:469-476, 2015.
