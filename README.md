# csgoHelper
CSGO Helper with GUI so you can be pro!

How to build: 

- Download Visual Studio with "Desktop development with C++"

1. Make a new Empty Project with c++ in visul studio.
2. Change configuration to "Release" and "x86".
3. Go to project properties (Configuration Properties > General) change "C++ Language Standard" to "ISO C++ 20 Standard (/std:c++20)".
4. Change output directory to "$(SolutionDir)\build\"
5. Go to Configuration Properties > Advanced and change Character Set to "Use Multi-Byte Character Set".
6. Go to Linker > System and change SubSystem to "Windows (/SUBSYSTEM:WINDOWS).
7. Go to Linker > Input and Additional Dependencies > Edit add "d3d11.lib".
8. Save changes.
9. Under Solution Explorer click "Show All Files".
10. Create a src folder and an external folder with an embedded imgui folder.
11. Download the source files as a ZIP.
12. Move the files from src in the zip to source in your project.
13. Move the files from external > imgui to external > imgui in your project.
14. Rightclick on the src folder and click on "Include In Project" do the same for the imgui folder.
15. Build solution and be pro in csgo!
