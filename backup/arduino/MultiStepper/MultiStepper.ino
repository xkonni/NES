// MultiStepper.pde
// -*- mode: C++ -*-
//
// Shows how to multiple simultaneous steppers
// Runs one stepper forwards and backwards, accelerating and decelerating
// at the limits. Runs other steppers at the same time
//
// Copyright (C) 2009 Mike McCauley
// $Id: MultiStepper.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

#include <AccelStepper.h>

// Define some steppers and the pins the will use
AccelStepper stepper1(AccelStepper::DRIVER, 8, 9);

void setup()
{
 stepper1.setMaxSpeed(100000.0);
  stepper1.setAcceleration(1000);
  stepper1.moveTo(100000);
}

void loop()
{
  // Change direction at the limits
  if (stepper1.distanceToGo() == 0)
  stepper1.moveTo(-stepper1.currentPosition());
  stepper1.run();
}
