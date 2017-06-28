Box2D-SFML2-Debug-Draw
======================

Debug Draw class for Box2D 2.3.1 in SFML 2

Box2D and SFML versions used:
Box2D 2.3.1 https://github.com/erincatto/Box2D
SFML 2.0 - 2.4.2 build - https://github.com/SFML/SFML/releases/tag/2.4.2

This code should work just fine with Box2D 2.3.1, as well as any SFML 2 version past RC

There's no special magic in using this class. Just include the SFMLDebugDraw.h and .cpp files into your project,
create an instance of SFMLDebugDraw and tell your Box2D world to use it by calling <tt>SetDebugDraw(&debugDrawInstance)</tt>

main.cpp
========

main.cpp contains an example of how Debug draw works. It's not a copy of the testbed, not even close, but you can create a few objects and fling them around.

![Screenshot](https://cloud.githubusercontent.com/assets/17878428/26623058/21336eb4-45f5-11e7-81a3-498d17d75309.png)

