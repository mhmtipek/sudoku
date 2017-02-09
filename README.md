# sudoku game
My sudoku game for mobile devices

C++ and Qt Framework used for development. Code has documentation. I want to add more details about implementation later.

Tested on platforms
-------------------
- Android
- iOS Simulator
- macOS
- Ubuntu

Features
--------
- 3 difficulty levels (easy, normal, hard)
- Boards are created in runtime. You can't finish all the boards :)
- Scoreboard

Compiling
---------
- C++11 supported compiler is needed
- Open with QtCreator, press run

TODO
----
- Change QSharedPointer to std::unique_ptr in search tree
- Manually delete tree nodes to prevent stack overflow at tree destruction
