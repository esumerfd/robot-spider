#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H

#include <Arduino.h>
#include <mock_body.h>
#include <multi_step_gait.h>
#include <logging.h>

/**
 * Test harness for running gait sequences on MockBody.
 *
 * Allows testing gait logic without real hardware by:
 * - Applying step data directly to MockBody
 * - Simulating time progression
 * - Logging state transitions
 * - Reporting pass/fail results
 */
class TestHarness {
  private:
    MockBody _mockBody;
    uint32_t _simulatedTimeMs;
    uint32_t _tickIntervalMs;
    uint32_t _maxIterations;

  public:
    TestHarness(uint32_t tickIntervalMs = 20, uint32_t maxIterations = 500)
      : _simulatedTimeMs(0),
        _tickIntervalMs(tickIntervalMs),
        _maxIterations(maxIterations) {}

    /**
     * Apply a single gait step to the mock body.
     */
    void applyStep(const GaitStep& step) {
      Log::println("TEST: Applying step '%s'", step.name);

      _mockBody.applyLeftFront(step.leftFront.shoulderDelta, step.leftFront.kneeDelta);
      _mockBody.applyLeftMiddle(step.leftMiddle.shoulderDelta, step.leftMiddle.kneeDelta);
      _mockBody.applyLeftRear(step.leftRear.shoulderDelta, step.leftRear.kneeDelta);
      _mockBody.applyRightFront(step.rightFront.shoulderDelta, step.rightFront.kneeDelta);
      _mockBody.applyRightMiddle(step.rightMiddle.shoulderDelta, step.rightMiddle.kneeDelta);
      _mockBody.applyRightRear(step.rightRear.shoulderDelta, step.rightRear.kneeDelta);
    }

    /**
     * Simulate time until all joints reach their targets.
     * Returns the number of iterations (ticks) taken.
     */
    uint32_t runUntilTarget() {
      uint32_t iterations = 0;

      while (!_mockBody.atTarget() && iterations < _maxIterations) {
        _mockBody.update(_tickIntervalMs);
        _simulatedTimeMs += _tickIntervalMs;
        iterations++;
        yield();  // Keep watchdog happy
      }

      return iterations;
    }

    /**
     * Run a complete gait sequence test.
     * Returns true if all steps completed successfully.
     */
    bool runGaitTest(const GaitSequenceData& gaitData) {
      Log::println("========================================");
      Log::println("TEST: Running gait '%s' (%d steps)", gaitData.name, gaitData.stepCount);
      Log::println("========================================");

      // Reset state
      _mockBody.resetToMiddle();
      _simulatedTimeMs = 0;

      Log::println("TEST: Initial state:");
      _mockBody.logState();

      // Run through each step
      for (uint8_t stepIndex = 0; stepIndex < gaitData.stepCount; stepIndex++) {
        const GaitStep& step = gaitData.steps[stepIndex];

        Log::println("----------------------------------------");
        Log::println("TEST: Step %d/%d: '%s'", stepIndex + 1, gaitData.stepCount, step.name);

        // Apply the step
        applyStep(step);

        // Log targets
        Log::println("TEST: After applying step:");
        _mockBody.logState();

        // Wait for completion if required
        if (step.waitForCompletion) {
          uint32_t iterations = runUntilTarget();

          if (!_mockBody.atTarget()) {
            Log::println("TEST FAILED: Step %d did not reach target after %d iterations",
                         stepIndex, _maxIterations);
            return false;
          }

          Log::println("TEST: Step %d completed in %d iterations (%d ms simulated)",
                       stepIndex, iterations, iterations * _tickIntervalMs);
        }

        Log::println("TEST: State after step %d:", stepIndex);
        _mockBody.logState();
      }

      Log::println("========================================");
      Log::println("TEST PASSED: Gait '%s' completed successfully", gaitData.name);
      Log::println("Total simulated time: %d ms", _simulatedTimeMs);
      Log::println("========================================");

      return true;
    }

    /**
     * Test the advance/isComplete state machine for a gait.
     * This tests the MultiStepGait logic independently.
     */
    bool runStateMachineTest(const GaitSequenceData& gaitData) {
      Log::println("========================================");
      Log::println("TEST: State machine test for '%s'", gaitData.name);
      Log::println("========================================");

      MultiStepGait gait(&gaitData);

      Log::println("TEST: Initial - step=%d, isComplete=%s",
                   gait.getCurrentStep(),
                   gait.isComplete() ? "true" : "false");

      // Reset mock body
      _mockBody.resetToMiddle();

      // Simulate the robot loop logic
      uint8_t loopCount = 0;
      const uint8_t maxLoops = gaitData.stepCount + 2;  // Allow some margin

      while (!gait.isComplete() && loopCount < maxLoops) {
        Log::println("TEST: Loop %d - applying step %d", loopCount, gait.getCurrentStep());

        // Apply current step
        const GaitStep& step = gaitData.steps[gait.getCurrentStep()];
        applyStep(step);

        // Simulate until target reached
        uint32_t iterations = runUntilTarget();
        Log::println("TEST: Target reached in %d iterations", iterations);

        // Check isComplete BEFORE advancing (as robot loop does)
        bool wasComplete = gait.isComplete();
        Log::println("TEST: Before advance - isComplete=%s", wasComplete ? "true" : "false");

        if (!wasComplete) {
          gait.advance();
          Log::println("TEST: After advance - step=%d, isComplete=%s",
                       gait.getCurrentStep(),
                       gait.isComplete() ? "true" : "false");
        }

        loopCount++;
      }

      if (gait.isComplete()) {
        Log::println("========================================");
        Log::println("TEST PASSED: State machine completed in %d loops", loopCount);
        Log::println("========================================");
        return true;
      } else {
        Log::println("========================================");
        Log::println("TEST FAILED: State machine did not complete after %d loops", loopCount);
        Log::println("========================================");
        return false;
      }
    }

    /**
     * Test the full robot loop behavior for a gait.
     *
     * This simulates what robot.cpp actually does:
     * 1. Apply gait
     * 2. Update until atTarget
     * 3. Check isComplete - if true, should stop
     * 4. If not complete, advance and reapply
     * 5. Repeat
     *
     * The test FAILS if:
     * - The gait doesn't complete within max loops
     * - Steps keep getting reapplied after gait should be complete
     */
    bool runRobotLoopTest(const GaitSequenceData& gaitData) {
      Log::println("========================================");
      Log::println("ROBOT LOOP TEST: '%s' (%d steps)", gaitData.name, gaitData.stepCount);
      Log::println("========================================");

      // Reset state
      _mockBody.resetToMiddle();
      _simulatedTimeMs = 0;

      MultiStepGait gait(&gaitData);
      gait.reset();

      Log::println("TEST: Initial state - step=%d, isComplete=%s",
                   gait.getCurrentStep(),
                   gait.isComplete() ? "true" : "false");
      _mockBody.logState();

      // Track how many times we apply each step (to detect infinite loops)
      const uint8_t MAX_STEP_APPLICATIONS = 3;
      uint8_t stepApplicationCounts[10] = {0};  // Support up to 10 steps

      uint32_t loopCount = 0;
      const uint32_t MAX_LOOPS = 100;
      bool isMoving = true;

      // Apply initial gait (like handleForwardCommand does)
      Log::println("TEST: === Starting movement ===");
      applyStep(gaitData.steps[gait.getCurrentStep()]);
      stepApplicationCounts[gait.getCurrentStep()]++;

      while (isMoving && loopCount < MAX_LOOPS) {
        // Simulate one tick of the robot loop
        _mockBody.update(_tickIntervalMs);
        _simulatedTimeMs += _tickIntervalMs;

        // Check if at target (like robot.cpp does)
        if (_mockBody.atTarget()) {
          uint8_t currentStep = gait.getCurrentStep();
          Log::println("TEST: Loop %d - atTarget=true, step=%d, isComplete=%s",
                       loopCount, currentStep, gait.isComplete() ? "true" : "false");

          if (!gait.isComplete()) {
            // Advance to next step (like robot.cpp does)
            gait.advance();

            // Check if NOW complete after advance
            if (gait.isComplete()) {
              Log::println("TEST: Gait complete after advance, transitioning to stationary");
              isMoving = false;
            } else {
              // Apply the new step
              uint8_t newStep = gait.getCurrentStep();
              Log::println("TEST: Advanced to step %d, applying...", newStep);

              // Check for infinite loop - same step applied too many times
              stepApplicationCounts[newStep]++;
              if (stepApplicationCounts[newStep] > MAX_STEP_APPLICATIONS) {
                Log::println("========================================");
                Log::println("TEST FAILED: Step %d applied %d times - INFINITE LOOP DETECTED",
                             newStep, stepApplicationCounts[newStep]);
                Log::println("========================================");
                return false;
              }

              applyStep(gaitData.steps[newStep]);
              _mockBody.logState();
            }
          } else {
            // Already complete
            Log::println("TEST: Gait was already complete, transitioning to stationary");
            isMoving = false;
          }
        }

        loopCount++;
        yield();
      }

      // Final state
      Log::println("----------------------------------------");
      Log::println("TEST: Final state after %d loops (%d ms):", loopCount, _simulatedTimeMs);
      Log::println("  isMoving=%s, step=%d, isComplete=%s",
                   isMoving ? "true" : "false",
                   gait.getCurrentStep(),
                   gait.isComplete() ? "true" : "false");
      _mockBody.logState();

      // Verify we stopped properly
      if (isMoving) {
        Log::println("========================================");
        Log::println("TEST FAILED: Still moving after %d loops", MAX_LOOPS);
        Log::println("========================================");
        return false;
      }

      if (!gait.isComplete()) {
        Log::println("========================================");
        Log::println("TEST FAILED: Gait not marked complete");
        Log::println("========================================");
        return false;
      }

      // Verify each step was applied exactly once
      Log::println("TEST: Step application counts:");
      bool countsOk = true;
      for (uint8_t i = 0; i < gaitData.stepCount; i++) {
        Log::println("  Step %d: applied %d times", i, stepApplicationCounts[i]);
        if (stepApplicationCounts[i] != 1) {
          countsOk = false;
        }
      }

      if (!countsOk) {
        Log::println("========================================");
        Log::println("TEST FAILED: Steps not applied exactly once each");
        Log::println("========================================");
        return false;
      }

      Log::println("========================================");
      Log::println("TEST PASSED: Robot loop completed correctly");
      Log::println("  Total loops: %d", loopCount);
      Log::println("  Total time: %d ms", _simulatedTimeMs);
      Log::println("========================================");
      return true;
    }

    // Access to mock body for custom tests
    MockBody& mockBody() { return _mockBody; }
    uint32_t getSimulatedTimeMs() const { return _simulatedTimeMs; }
};

#endif
