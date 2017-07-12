classroom face detection
========================

### Build

fetch Mini-Caffe code first.

```
$ git submodule update --init
```

On Windows with Visual Studio 2013.

```
$ mkdir build
$ cd build
$ cmake .. -G "Visual Studio 12 2013 Win64"
```

On Linux with CUDA and cuDNN support.

```
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_CUDA=ON -DUSE_CUDNN=ON
```
