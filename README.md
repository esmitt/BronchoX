BronchoX
===================
[comment]: <> (Using the [<i class="icon-file"></i> Google C++ Documentation](https://google.github.io/styleguide/cppguide.html) to follow a standard guideline)
For now, mixing VTK and OpenCV in a good way. The idea is to extract frames from render in order to be manipulated by OpenCV

----------
#### Initial Setup
- [VTK][1] version 8.0.1 (October 2017)
- [Microsoft Visual Studio Community 2017][2] v 8.1

Now, partially it is necessary to add the following instruction into Visual Studio (Debugging -> Environment):
`PATH=%PATH%;C:/opencv/build/x64/vc14/bin;%VTK_DIR%`

[1]: https://www.vtk.org
[2]: https://www.visualstudio.com/?sku=Community&rel=15