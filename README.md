# GEditor-cpp
*Lightweight video editor V2*

Requirements
------------
  - Windows 10
  - CMake 3.15+
  - Visual Studio 2019
  - Qt 5.14.2 Prebuilt components for MSVC 2017
  - Clang 10.0.0
  - Ninja v1.10.0

Build
##### 1. Rename config.local.example to config.local and replace Qt path with your own Qt installation path.

------------
##### 2. Set up MSVC toolchain environment in Windows cmd as shown below or use the "Developer Command Prompt for VS 2019"
```
cd ${PATH_TO_VISUAL_STUDIO}\VC\Auxiliary\Build
vcvars64.bat
```
##### 3. Navigate to a folder you want to build in using the command prompt
```
git clone https://github.com/SpreeHD/GEditor-cpp.git GEditor-cpp
cd GEditor-cpp
mkdir build
cd build
cmake -G Ninja -DCMAKE_C_COMPILER="${PATH_TO_CLANG}/clang-cl.exe" -DCMAKE_CXX_COMPILER="${PATH_TO_CLANG}/clang-cl.exe" -DCMAKE_C_FLAGS=-m64 -DCMAKE_CXX_FLAGS=-m64 ..
cmake --build .
Move debug binaries from the releases section to your build folder
```

Functions
------------

![](https://i.imgur.com/xUsbaSM.png)

1. **Choose LUT:** Changes the colors of a video file. Download a LUT (Lookup table) .cube file and apply it to your video.  
2. **Change Codec:** Changes the codec of a video file. **NB! NVENC codec only works with Nvidia GPUs**  
3. **Change Speed:** Changes the speed of a video file. (up to 5x speed)
4. **Extract Subclip:** Extracts a clip with the desired timestamps from a video file.  
5. **Scale Video:** Scales a video file. (100x100 to 1920x1080)
6. **Convert to MP3:** Converts a video file to an .mp3 file.  
7. **Save as GIF:** Converts a video to a .gif.  
8. **Stabilize Video:** Stabilizes a video. (Takes a while)  
9. **Upload to streamable:** Upload your imported video to Streamable.  
10. **Custom Commands:** You're able to execute custom ffmpeg commands with GEditor. Enter a command you would execute with ffmpeg normally. Example: "-i input.mp4 output.webm".  

#### *Refer to the console for ffmpeg error messages.*
