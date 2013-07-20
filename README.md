Box2D-SFML2-Debug-Draw
======================

Debug Draw class for Box2D 2.2.1 - 2.3.0 in SFML 2

Box2D and SFML versions used:
Box2D 2.3.0 r254 - http://code.google.com/p/box2d/source/browse/?r=254
SFML 2.0 build[5c46daacd4] - https://github.com/LaurentGomila/SFML/tree/5c46daacd4e95ad0dc46bfbcabd1d7adfe01bbc0

This code should work just fine with Box2D 2.2.1, as well as any SFML 2 version past RC

There's no special magic in using this class. Just include the SFMLDebugDraw.h and .cpp files into your project,
create an instance of SFMLDebugDraw and tell your Box2D world to use it by calling <tt>SetDebugDraw(&debugDrawInstance)</tt>

main.cpp
========

main.cpp contains an example of how Debug draw works. It's not a copy of the testbed, not even close, but you can create a few objects and fling them around.

![Screenshot](http://imgur.com/zqcm5MB)

