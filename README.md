# Elevator Madness

It's a busy workday in a high office building and people, unaware of the pointless nature of their existence are rushing up and down its floors.

They all have very important stuff to do and can't afford to take the stairs or wait a bit more, so they find the manual elevator controls quite innefficient.

Can you rise up to the challenge and invent the most ingenious algorithm for elevator control?


## Rules

Each level, represents a unique office building. It may have different floors and number of elevators, with each of them servicing different range of floors and with different capacity.

The game plays in turns and on each turn you will receive the following info:
  - each elevator's speed and position (height in meters above floor 0)
  - the number of people waiting on a given floor, their desired destination and the number of turns, they are willing to wait for an elevator.

When a person calls an elevator you must respond with an elevatorId, which the person will wait to get in. You cannot change this assignment later.
A person remembers his elevator and waits for it, until the specific elevator stops at his floor, or until his patience runs out.

The elevators are controlled by motors, that can be given one of two actions - acellerated up or accelerated down. When an elevator stops (speed = 0) at a given floor, the doors open and people can get in and out.

Floors always start from 0 (as every true countable thing should).
A floor is 4 meters high and elevators can accelerate either up or down with 2 m/s<sup>2</sup>


### Input Format
Initialization input:

Line 1:
the number of `FloorsNb` and `ElevatorsNb` in the building

Next `ElevatorsNb` lines:

`ElevatorId` - a uniqie identifier for an elevator,

`MinFloor` and `MaxFloor` - the range of floors that the elevator can move between

`Capacity` - the number of people that can fit at once

Input for one game turn:

Line 1:

number of calling `CallingPeopleNb`, waiting `WaitingPeopleNb` and `PeopleInsideElevatorsNb`

Next `ElevatorsNb` lines provide information about each elevator:

`ElevatorId` - unique identifier of the elevator, single character

`Position` - position in meters above floor 0, divisible by 2

`Speed` - either positive or negative number divisible by 2

`PeopleNb` - number of people inside the elevator

Next `CallingPeopleNb` lines provide information about people that are calling an elevator:

`SourceFloor` - where the person is located

`DestinationFloor` - where the person wants to go

Next `WaitingPeopleNb` lines provide information about people that are waiting for their elevator:

`ElevatorId` - which elevator has been assigned to this person

`TurnsToWait` - the number of game turns until the person becomes annoyed and decides to give up on waiting the elevator.

Next `PeopleInsideElevatorsNb` lines:

`ElevatorId` - in which elevator the person is located

`DestinationFloor` - where the person wants to go
