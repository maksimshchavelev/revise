# Spaced repetition app

The user manual is available in the following languages:
 - [Русский](docs/ru/usage.md)


## Building for Linux

To build the application, you will need Qt 6.10.1 compiled with **WebEngine** support. The remaining dependencies should be downloaded automatically. During configuration, you can specify the following CMake **variables**:

- `LINUXDEPLOYQT` points to the linuxdeployqt executable. Once you've set this variable, you can run the following command after the build to create an AppImage: `cmake --build . --target appimage`. 

The following options are also **available**:

- `REVISE_BUILD_TESTS` - Should tests be built? (`ON` / `OFF`)
- `REVISE_BUILD_APP` - Should the application itself be built? (`ON` / `OFF`)

##  Third parties

The project uses the following libraries (in addition to Qt):

- [zstd](https://github.com/facebook/zstd)
- [quazip](https://github.com/stachenov/quazip.git)
