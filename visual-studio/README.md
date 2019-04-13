Compiling for Visual Studio
============================

The solution file (midifile.sln) contains the project for the static 
library in /src (midifile.vcxproj) as well as projects for
individual projects for client programs in /tools.

The projects are configured for the v140 (Visual Studio 2015) toolset
by default, but this can be changed.

As new programs are added to /tools, corresponding project
files should be created for them in this directory, and then added
to midifile.sln. The easiest was of creating a new project file
is to copy an existing project file and then rename all occurrences
of the old name with the new one.

