#include "multi_step_gait.h"
#include <Arduino.h>

MultiStepGait::MultiStepGait(const GaitSequenceData* data)
  : _sequenceData(data),
    _currentStepIndex(0),
    _stepInProgress(false),
    _applyProfiler("GaitApply", false, 1000) {  // Disabled by default, log every 1s
}

void MultiStepGait::applyTo(LeftFrontLeg& leg) {
  const GaitStep& step = _sequenceData->steps[_currentStepIndex];
  applyLegMovement(leg, step.leftFront);
}

void MultiStepGait::applyTo(LeftMiddleLeg& leg) {
  const GaitStep& step = _sequenceData->steps[_currentStepIndex];
  applyLegMovement(leg, step.leftMiddle);
}

void MultiStepGait::applyTo(LeftRearLeg& leg) {
  const GaitStep& step = _sequenceData->steps[_currentStepIndex];
  applyLegMovement(leg, step.leftRear);
}

void MultiStepGait::applyTo(RightFrontLeg& leg) {
  const GaitStep& step = _sequenceData->steps[_currentStepIndex];
  applyLegMovement(leg, step.rightFront);
}

void MultiStepGait::applyTo(RightMiddleLeg& leg) {
  const GaitStep& step = _sequenceData->steps[_currentStepIndex];
  applyLegMovement(leg, step.rightMiddle);
}

void MultiStepGait::applyTo(RightRearLeg& leg) {
  const GaitStep& step = _sequenceData->steps[_currentStepIndex];
  applyLegMovement(leg, step.rightRear);
}

const char* MultiStepGait::getName() const {
  return _sequenceData->name;
}

void MultiStepGait::applyLegMovement(Leg& leg, const LegMovement& movement) {
  _applyProfiler.tick();

  if (movement.shoulderDelta != 0) {
    applyDelta(leg.shoulder(), movement.shoulderDelta, movement.duration);
  }
  if (movement.kneeDelta != 0) {
    applyDelta(leg.knee(), movement.kneeDelta, movement.duration);
  }
}

void MultiStepGait::applyDelta(Joint& joint, int16_t delta, uint16_t duration) {
  // Calculate new target as current position + delta
  uint16_t currentPos = joint.getPosition();
  int32_t newTarget = currentPos + delta;

  // Clamp to safe servo range
  const uint16_t safeMin = _board.servoMin() + 5;
  const uint16_t safeMax = _board.servoMax() - 5;
  newTarget = constrain(newTarget, safeMin, safeMax);

  // Calculate speed from distance and duration
  uint16_t distance = abs(delta);
  uint16_t speed = (distance * 1000) / duration;  // units per second

  joint.setTarget(newTarget, speed);
}

void MultiStepGait::advance() {
  // Check if current step is complete
  if (_stepInProgress && _sequenceData->steps[_currentStepIndex].waitForCompletion) {
    // Caller should check body.atTarget() before calling advance()
    return;
  }

  _currentStepIndex++;

  // Handle looping
  if (_currentStepIndex >= _sequenceData->stepCount) {
    if (_sequenceData->looping) {
      _currentStepIndex = 0;
    } else {
      _currentStepIndex = _sequenceData->stepCount - 1;  // Stay on last step
    }
  }

  _stepInProgress = false;
}

bool MultiStepGait::isComplete() const {
  return !_sequenceData->looping &&
         _currentStepIndex >= _sequenceData->stepCount - 1 &&
         !_stepInProgress;
}

void MultiStepGait::reset() {
  _currentStepIndex = 0;
  _stepInProgress = false;
}

uint8_t MultiStepGait::getCurrentStep() const {
  return _currentStepIndex;
}

void MultiStepGait::updateProfiler(uint32_t currentMs) {
  _applyProfiler.update(currentMs);
}

void MultiStepGait::enableProfiling(bool enabled) {
  _applyProfiler.setEnabled(enabled);
}

CallRateProfiler& MultiStepGait::getProfiler() {
  return _applyProfiler;
}
