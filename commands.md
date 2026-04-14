
# Command Table

```
0b TSMM MPPP [VVVV VVVV [VVVV VVVV [VVVV VVVV [VVVV VVVV [VVVV VVXX]]]]]

T - toggle
S - New State
M - Module Select bits (which lighting module to control)
P - Param Byte Num (how may parameter bytes follow) should only specify up to 5 (0b101) but will pull as many as specified
X - Ignored
```

## Module Select table:

| `MMM` | Module name             | Param Behavior |
| :---: | :---------------------- | :------------- |
| `000` | Cockpit Main Lights     | None           |
| `001` | Rear Compartment Lights | None           |
| `010` | Cockpit Console Left    | None           |
| `011` | Cockpit Console Right   | None           |
| `100` | Engine pods             | None           |
| `101` | DHD                     | Button Select  |

## DHD Button Select Behaviour

General key:
- `X` - ignored
- `_` - Not present
- `V` - Value
- `R` - Row
- `C` - Column

| `PPP` | bit map | Behaviour                                                                                            |
| :---: | :------------------------------------------------------ | :--------------------------------------------------------------------------------------------------- |
| `000` | `____ ____  ____ ____  ____ ____  ____ ____  ____ ____` | `TS` affects all buttons                                                                             |
| `001` | `RRR0 XXXX  ____ ____  ____ ____  ____ ____  ____ ____` | `TS` affects whole row                                                                               |
| `001` | `RRR1 CCCC  ____ ____  ____ ____  ____ ____  ____ ____` | `TS` affect specific button                                                                          |
| `010` | `0RRR XXXV  VVVV VVVV  ____ ____  ____ ____  ____ ____` | `TS` affects specified buttons in row (if both `T` and `S` are HI sets row to specified state)       |
| `010` | `1CCC CXXX  XXVV VVVV  ____ ____  ____ ____  ____ ____` | `TS` affects specified buttons in column (if both `T` and `S` are HI sets column to specified state) |
| `011` | UNDEFINED                                               | UNDEFINED                                                                                            |
| `100` | UNDEFINED                                               | UNDEFINED                                                                                            |
| `101` | `VVVV VVVV  VVVV VVVV  VVVV VVVV  VVVV VVVV  VVVV VVXX` | `TS` affects specified buttons in array (if both `T` and `S` are HI sets array to specified state)   |
| `110` | UNDEFINED                                               | UNDEFINED                                                                                            |
| `111` | UNDEFINED                                               | UNDEFINED                                                                                            |