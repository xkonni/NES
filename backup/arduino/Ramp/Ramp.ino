/*
 * Ramp.ino
 */

// steps per second
/* int speedMin = 1; */
/* int speedMax = 1000; */
double speedCur = 0;

// clocks
int t_clock, t_zero;
int t_one = 1;

void accelerate(int nSteps, int speed) {
  double step = speed/nSteps;
  int i;
  for (i = 0; i < nSteps; i++) {
    // update speed
    speedCur += step;
    // calculate clocks
    t_clock = 1e6 / speedCur;
    t_zero = t_clock - t_one;

    // step
    digitalWrite(8, HIGH);
    delayMicroseconds(t_one);
    digitalWrite(8, LOW);
    delayMicroseconds(t_zero);
  }
}

void decelerate(int nSteps, int speed) {
  double step = (speedCur - speed)/nSteps;
  int i;
  for (i = 0; i < nSteps; i++) {
    // update speed
    speedCur -= step;
    // calculate clocks
    t_clock = 1e6 / speedCur;
    t_zero = t_clock - t_one;

    // step
    digitalWrite(8, HIGH);
    delayMicroseconds(t_one);
    digitalWrite(8, LOW);
    delayMicroseconds(t_zero);
  }
}

void run() {
  // calculate clocks
  t_clock = 1e6 / speedCur;
  t_zero = t_clock - t_one;

  // step
  digitalWrite(8, HIGH);
  delayMicroseconds(t_one);
  digitalWrite(8, LOW);
  delayMicroseconds(t_zero);
}

void setup()
{
  // initialize pins
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  // get up to speed
  accelerate(10, 5000);
  /* int i; */
  /* for (i = 0; i < 10000; i++) { */
  /*   run(); */
  /* } */
  /* decelerate(1500, 500); */
  /* for (i = 0; i < 10000; i++) { */
  /*   run(); */
  /* } */
  /* accelerate(150, 2000); */
  /* for (i = 0; i < 10000; i++) { */
  /*   run(); */
  /* } */
  /* decelerate(150, 500); */
  /* for (i = 0; i < 10000; i++) { */
  /*   run(); */
  /* } */
}

void loop()
{
  run();
}
