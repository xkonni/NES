:title: NES Project: Position Sensing and Imitation - Intermediate Presentation

:author: Moksha Birk, Konstantin Koslowski, Mathis Schmieder
:description: Intermediate Presentation
:keywords: presentation, NES


Reminder: Goal Statement
========================

**Goal:** Mimic position and motion of a plate
----------------------------------------------

* **Sensing:** 3D MEMS attitude sensor embedded in a plate
* **Communicating:** Implement industrial bus
* **Actuating:** Rotate a plate using motors

----

Reminder: Functional Overview
=============================

.. image:: functionalspecification.png

----

Reminder: Major Milestones
==========================

* **Sensing:** Read and process MEMS data
* **Actuation:** Control stepper motors
* **Mechanics:** Construct movable plate
* **Communication:** Implement industrial bus
* **Controller:** Bus master, main computational unit

----

Milestone: Sensing
==================
Read and process MEMS data
--------------------------
**Status:**

* Reading data via I2C works
* Computing plate position from data works
* Additional filtering might be required

----

Milestone: Actuation
====================

Control stepper motors
----------------------

**Status:**

* Communication with stepper drivers via SPI works
* Control of stepper motors works
* Additional work on control daemon necessary

----

Milestone: Mechanics
====================

Construct movable plate
-----------------------

**Status:**

* First version of plate construction printed
* Works for now
* Design on second, refined version in progress

----


Milestone: Communication
========================

Implement industrial bus
------------------------

**Status:**

* A lot of research was done
* EtherCAT selected as most interesting
* CAN selected as fallback
* Work in progress

----

Milestone: Controller
=====================

Bus master, main computational unit
-----------------------------------

**Status:**

* Modular design to fit CAN and EtherCAT
* High-level controller class
    * Receives periodic sensor input events
    * Computes angle corrections for all drives
* CAN or EtherCAT wrapped into classes to provide the events and send corrections
* Built on a BeagleBone Black

----

Timing
======

**Timing goal:** Move plate to desired position within 1 second

**Fixed timings:**

* Sensors
    * Sample every 10 ms
    * Report mean value every 100 ms
* Actuation takes up to 500 ms

**Delay constraint:** 500 ms to compute & communicate

----

Message Description
===================

::

  Description       | Data Request  | Length
  -------------------------------------------
  Sensor Position   | Allowed       | 6 Bytes
  Motor Status      | Allowed       | 4 Bytes
  Rotation Command  | Not Allowed   | 3 Bytes
  Reset Command     | Not Allowed   | 3 Bytes

*Possible messages in the network*

----

Future work
===========

* Implement bus communication
* Finish master controller
* Tune motor timings
* Refine plate construction

----

Thanks for your attention!
==========================

**Questions? Ideas? Suggestions?**
