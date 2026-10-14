# Cat And Mouse

## Building

Make sure the mingw-w64-toolchain is installed.

```
  mkdir build_windows && cd build_windows
  cmake -DCMAKE_TOOLCHAIN_FILE=../mingw-w64-toolchain.cmake ..
  make -j${ncores}
```

# Game Architecture

## Goal

The cat has to rid the house of mice.  If the mice irritate the human then it will throw the cat outside.  If the cat gets thrown outside three times then they will take the cat to the pound.  Game over!

## Characters

### Cat

The player is the cat.  The cat wants to sleep in a sunbeam, or maybe scratch the couch, or maybe lick the laundry basket, but if a mouse shows up, murder is all he thinks about.

#### Cat Capabilities

* basic movement
  * left
  * right
  * up
  * down
  * move/flip furniture
* pick up item (one at a time)
* drop item
* use item
* basic attack (without item)
* meow

### Mouse

The mouse is just looking for a nice place to call his own.  He likes food, shelter, and more food.  He knows the house isn't big enough for him and the cat.  The cat must be dealt with.

#### Mouse Capabilities

* basic movement
  * left
  * right
  * up
  * down
* avoid the cat
* basic attack (without item)
* pick up item (one at a time)
* drop item
* use item
* squeak

### The Human

The human owns the cat, and consider it the cat's job to rid the house of mice.  If the mouse irritates the human it will throw the cat outside.

The human moves fluidly from room to room. 

## Items

The items will stun either the cat or the mouse for a period of time.  During this time the creature is vulnerable to the wiles of the other.

* slingshot 
  * comes with 10 shots
  * stuns for 5 seconds
* laser blaster 
  * comes with 5 shots
  * stuns for 10 seconds
* toy robots
  * 5 shot laser blaster
  * 5 second stun
  * 30 second max wind-up

## The House

The game is played in rooms of the house. The furniture of each room is appropriate to the room type.  The cat and mouse will chase each other from one room to the next.  Each room has a chance to have the humans in.  If the humans are in the room then the mouse/mice has/have a chance to irritate the humans.  This is bad for the cat since they will kick the cat outside if they get irritated.

### Rooms and Furniture

Moveable items can be moved when the cat nudges against it.
Flipable items can be flipped when the cat nudges against it.  Then the cat can move against it to move it.
The cat can move the furniture to set traps for the mouse.

The mouse can not move or flip furniture.

Each room can have a number of toy robots in it.  Toy robots can be wound up by either the cat or the mouse.  If the mouse winds up a tor robot it will target the cat.  If the cat winds up the toy robot it will target the mouse.

When a room loads there is a chance the humans will be in it.

When a room loads there will be a random number of toy robots in it.

When a room loads there will be a random number of laser blasters in it.

If the human hear the mouse squeak they will throw the cat outside if the cat is in the same room.

* living room
  * door to kitchen
  * door to hallway
  * coffee table
    * flippable/moveable
  * couch
    * moveable
  * lamp
    * moveable
  * TV
    * moveable
  * bookcase
    * flippable/moveable
  * chair
    * flippable/moveable
* master bedroom
  * door to hallway
  * bed
  * night stand
    * moveable
  * cabinet
    * moveable
  * chest of drawers
    * moveable
  * chair
    * flippable/moveable
* guest bedroom
  * door to hallway
  * bed
  * night stand
    * moveable
  * chest of drawers
    * moveable
* kitchen
  * door to living room
  * stove
  * counter
  * refridgerator
  * dining table
    * flippable/moveable
  * four chairs around dining table
    * flippable/moveable
  * pantry
    * moveable

* hallway
  * door to living room
  * door to master bedroom
  * door to guest bedroom
  * night stand
    * moveable
  * lamp
    * moveable

## Game Play

* Game starts with all rooms with their furniture arranged in default setup
* Cat starts asleep in a random room
* Mouse runs at cat
* Cat wakes and starts the chase
* If the cat catches a mouse then he kills it he gets a point
* Mice spawn at random intervals.  There can potentially be multiple mice going at once
* There can be mice in rooms the cat is not in.  A HUD will display how many mice are in the house
* If a human is in the room with the cat and it hears the mouse squeak, or it gets hit by the mouse, then it will kick the cat out of the house and it looses a chance.
* If a mouse is still and hears the cat meow it will run.  The cat can use this to chase the mouse out of a room with a human in.
* If the cat is stunned by an item then the mice can run around and find the human.
* If a mouse is stunned by an item then it cannot run from the cat and has a chance at getting killed.
* The cat can move the furniture in the room to trap the mice.

## Game Board

* HUD
  * Scores
    * Mice killed
    * Chances remaining
  * Room maps
    * number of mice
    * human in (yes/no)
* Main screen
  * current room
