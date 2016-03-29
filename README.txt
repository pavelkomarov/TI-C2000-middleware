TIC2000: This should be your workspace when developing code, as all include paths are relative to WORKSPACE_LOC.

F28069Common contains asm, cmd, c, and h comprising TI's abstraction layer. These files are shared and a subset reused by all projects.

System Libraries is a secondary abstraction layer, largely by Pavel, which greatly simplifies interaction with C2000 systems, like GPIO, clocks, and the ADC.

Project Libraries contains a few tertiary abstractions, useful for simplifying projects.

Test Projects are written to test the functionality of various System or Project libraries. When creating final, functional projects, you will find the examples herein useful.

See also http://solarracing.gatech.edu/wiki/Main_Page for more in-depth descriptions of all these projects and how to build and use them.
