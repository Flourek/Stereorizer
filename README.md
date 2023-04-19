# Stereorizer

Easily add realistic depth perception to **any** image you want by generating a stereoscopic pair for viewing in VR

## Depth map generation
The view from the other eye is based on a depth map, where brighter values mean the pixel is closer, and therefore it is offset by a larger value. <br/>
It's automatically generated using machine learning - [MiDaS](https://github.com/isl-org/MiDaS).

You may also choose to author the depth map yourself and use that instead. 

The quality of the final result depends entirely on the quality of this depth map. 

## Inpainting
It's hard to tell what should be in the newly exposed areas that aren't in the original image. <br/>

Select from a few different inpainting methods which are based on the surrounding pixels. 

Instead of that you may export a mask of these areas and use that to paint in these areas in your favourite image editing software, however in most cases the stereo deviation is too small for this to be necessary.


## SteamVR Viewer
Using the built in viewer you can preview the image you're currently working on with changes being reflected in real-time. <br/> 
It's pretty barebones so after you've dialed everything you should probably export and use the viewer of your choice. 

## Installation
Prerequisites: Python 3.0

* Download the latest release
* Run `Install.bat`, which will: 
  * Make sure Python is installed
  * Install MiDaS' dependencies 
  * Download [dpt_beit_large_512.pt](https://github.com/isl-org/MiDaS/releases/download/v3_1/dpt_beit_large_512.pt) and put it in MiDaS/models
* Done :)

## Building
Code was tested using Python 3.10.6, OpenGL 4.6.0

Build: `cmake make idk`

This repository is also a CLion project.

