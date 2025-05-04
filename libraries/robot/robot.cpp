
#include <robot.h>
#include <flasher.h>
#include <logging.h>

#include <body.h>
#include <servo.h>
#include <board.h>
#include <sequence.h>
#include <mover_up.h>

Flasher flasher;

Board board;
Servo servo = Servo(board, 0);
MoverUp mover = MoverUp(board.servoMin(), board.servoMax());
Sequence sequence = Sequence(servo);
Body body = Body(servo, sequence);
byte endMarter;

void Robot::setup() {
  Log::begin();
  flasher.begin();

  delay(500);

  // Construct model
  // new Knee
  // new Sholder
  // new Leg

  sequence.add(mover);

  body.begin();

  // report status of memory, devices.
}

void Robot::loop() {
  flasher.flash();

  // check queue for operation.
  // take next opertion step.
  body.action();

  Log::println(
      "Robot setup: body: 0x%x (%d), sequence: 0x%x (%d), mover 0x%x (%d), servo 0x%x (%d)--",
      &body, sizeof(Body), &sequence, sizeof(Sequence),
      &mover, sizeof(MoverUp), &servo, sizeof(Servo));

  Log::println("Status: mover count %d", sequence.size());

  delay(100);
}
