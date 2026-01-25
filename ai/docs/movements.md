# Robot Spider Movement Rules

## Joint Naming Convention

Each leg has two joints: shoulder and knee.

| Pin | Joint Name |
|-----|------------|
| 0 | LF.Shoulder (Left Front) |
| 1 | LF.Knee |
| 2 | LM.Shoulder (Left Middle) |
| 3 | LM.Knee |
| 4 | LR.Shoulder (Left Rear) |
| 5 | LR.Knee |
| 6 | RF.Shoulder (Right Front) |
| 7 | RF.Knee |
| 8 | RM.Shoulder (Right Middle) |
| 9 | RM.Knee |
| 10 | RR.Shoulder (Right Rear) |
| 11 | RR.Knee |

## Knee Movement Rules

The knee controls the vertical position of that leg corner.

| Side | Negative (-) | Positive (+) |
|------|--------------|--------------|
| Left (LF, LM, LR) | DOWN | UP |
| Right (RF, RM, RR) | UP | DOWN |

**Example:** To lower all knees (extend legs, lift body):
- Left knees: use negative values (-23°)
- Right knees: use positive values (+23°)

## Shoulder Movement Rules

The shoulder controls the forward/backward swing of the leg.

| Side | Negative (-) | Positive (+) |
|------|--------------|--------------|
| Left (LF, LM, LR) | BACKWARD | FORWARD |
| Right (RF, RM, RR) | FORWARD | BACKWARD |

**Example:** To swing all shoulders forward:
- Left shoulders: use positive values (+10°)
- Right shoulders: use negative values (-10°)

## Tripod Groups

For walking gaits, legs are grouped into two tripods that alternate:

- **Tripod A:** Left Front, Left Rear, Right Middle
- **Tripod B:** Right Front, Right Rear, Left Middle

## Movement Data Format

Each leg movement is specified as: `{shoulderDelta, kneeDelta, duration}`

- `shoulderDelta`: Relative angle change for shoulder (0 = no movement)
- `kneeDelta`: Relative angle change for knee (0 = no movement)
- `duration`: Time in milliseconds (0 = use constant speed)
