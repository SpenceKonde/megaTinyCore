# CCL Toolbox

There are some simple patterns that you can use with the CCL/Logic library to generate very widly applicable effects:

In the below examples X, Y, Z and 2 are used to refer to inputs. X, Y, and Z can refer to any input, but for the purposes of the LUT presented, X = 0, Y = 1, Z = 2

Input 2, since it can be used as a clock, may be specifically required.

## Oscillation
You can make a CCL oscillate (and this is used in prescaling of clocks, see the bottom section).

X: Feedback
Y: masked
Z: masked
Clock: N/A
LUT:
000: 1
001: 0

TRUTH = 0x01

With no clock, this will oscillate at around 80-100 MHz and is highly sensitive to conditions.

It's much more useful if you put a clock source in, typically the system clock - See below for how to use this as a clock prescaler.

## Switch
X: Input A
Y: Input B
Z: Select
Clock: as dictated by application

LUT:
000: 0
001: 1
010: 0
011: 1
100: 0
101: 0
110: 1
111: 1
TRUTH = 0xCB
Will output whatever input 0 is if input 2 is low, and whatever input 1 is if input 2 is high.

## PWM is magic
Unlike the event channels, where you get a single clock long pulse from a compare match or overflow, the inputs to logic blocks are the level of the output compare! That means that you can remap pins that don't exist on your part but would have PWM if they did to the LUT output pin.

Combine with count on event to switch between two inputs based on the value of the counter,


## Feedback
The "Feedback" channel is the **output of the sequencer if used, and the even LUT in that pair if not**,

### But I need feedback on an ODD LUT
"Ya can't get there from here" The only way to do this is he price in the form of one event channel: Use the output as a generator, and set one of the event inputs of the CCL block to that channel

## Sequential logic with just one LUT
You can simulate some sequential logic units with just one LUT!

Enable the synchronizer to get the analogous flip-flop.

### S-R latch
X: Feedback
Y: Set (any input source)
Z: Clear (any input source)
Clock: N/A

LUT:
* 000: 0
* 001: 1
* 010: 1
* 011: 1
* 100: 0
* 101: 0
* 110: Per application requirements - logic block is getting contradictroy signals
* 111: Per application requirements - logic block is getting contradictroy signals
Ergo: TRUTH = 0x0b??001110 = 0x07, 0xC7. Using 0x47 or 0x87 will result in high speed oscillation, which is rarely desirable.

### D-Type latch
X: Feedback
Y: D (gated signal to latch)
Z: G (Gate)
Clock: N/A

LUT:
* 000: 0
* 001: 1
* 010: 0
* 011: 1
* 100: 0
* 101: 0
* 110: 1
* 111: 1
Ergo: TRUTH = 0xCB

## Prescaling clocks with the CCL

You can use CCL logic blocks to prescale a clock, albeit inefficiently. On parts with 6 LUTs, you can prescale by 2^18 if you're willing to use all LUTs and 3 event channels. This can be used for example to clock a TCB from a prescaled value that is slower than half the system clock but not used by any TCA.

Generally, I would argue that if you're using more than 3 LUTs, you should consider whether there is any other way to achieve your goal.

## The general scheme

1. Find the prescaling factor you want on the table below.
2. Check the LUTS and EVENTS columns and make sure you have that many available.
  a. An additional event channel will be needed if you want to "skip" the more useful even lut in a pair\
  b. If ALL luts are used, and the chip is not impacted by errata about the link input on LUT3, AND you don't need the output on an event channel, you can save a channel. You can also save a channel if only using 2 LUTs.
3. Select a block of the necessary number of consecutive LUTs,
  a. The lowest LUT should be even.
4. Configure the LUTs:
  a. The highest number LUT should be set to use the output of the lowest number LUT, either through feedback (if only 2 luts used), link (if all luts are used for this) or otherwise an event channel set to use the lowest number LUT as generator.
  b. If the number under the column with the LUT number heading (0-5) is a 4, the "filter/synchronizer" should be set as synchronizer.
  c. If the number is an 8, it should be set as a filter.
  d. The highest number LUT should use the system clock (or other clock to be prescaled.
  e. If the operation is ADD, ADD2, ADD2MUL3, ADD4, or ADD4MUL5, set input 0 that lut to link, and the truth table to 0x02. (output a 1 when input is 1)
  f. If the operation in mul, set input 0 to be feedback (if even) or an event channel carrying it's own output (if odd), and input 2 to link (unless it's the highest lut),
  g. The highest LUT, and any luts multiplying the previous one, should get a truth table of 0x01 (output a 1 when input is 0).



## The giant table

| Total | p0 | 0 | 1 | oppr 0    | oppr 1    | p2+3 | p2 | 2 | 3 | oppr 2    | oppr 3    | p3 | 4 | 5 | op4 | LUTS | Events |
|-------|----|---|---|-----------|-----------|------|----|---|---|-----------|-----------|----|---|---|-----|------|--------|
|     4 |  4 | 4 | - |         - |         - |    - |  - | - | - |         - |         - |  - | - | - |   - |    2 |      0 |
|     8 |  8 | 4 | - |         - |         - |    - |  - | - | - |         - |         - |  - | - | - |   - |    2 |      0 |
|    12 | 12 | 4 | 8 |       ADD |         - |    - |  - | - | - |         - |         - |  - | - | - |   - |    2 |      0 |
|    16 | 16 | 8 | 8 |       ADD |         - |    - |  - | - | - |         - |         - |  - | - | - |   - |    2 |      0 |
|    20 | 16 | 4 | 4 |      ADD2 |       ADD |    - |  4 | 4 | - |       ADD |         - |  - | - | - |   - |    3 |      1 |
|    24 | 16 | 4 | 4 |      ADD2 |       ADD |    - |  8 | 8 | - |       ADD |         - |  - | - | - |   - |    3 |      1 |
|    28 |  8 | 4 | 4 |       ADD |       ADD |   20 |  8 | 4 | 4 |       ADD |       ADD | 12 | 4 | 8 | ADD |    6 |      0 |
|    32 | 32 | 4 | 8 |       MUL |         - |    - |  - | - | - |         - |         - |  - | - | - |   - |    2 |      0 |
|    36 |  8 | 4 | 4 |       ADD |       ADD |   28 | 12 | 4 | 8 |       ADD |       ADD | 16 | 8 | 8 | ADD |    6 |      0 |
|    40 |  8 | 4 | 4 |       ADD |       ADD |   32 | 16 | 8 | 8 |       ADD |       ADD | 16 | 8 | 8 | ADD |    6 |      0 |
|    44 | 12 | 4 | 8 |       ADD |  ADD2MUL3 |    - |  * | 4 | 8 |       ADD |         - |  - | - | - |   - |    4 |      2 |
|    48 | 16 | 4 | 4 |      ADD2 |       ADD |    - | 32 | 4 | - |       ADD |  ADD4MUL5 |  * | 4 | 4 |   - |    5 |      1 |
|    52 | 32 | 4 | 8 |       MUL |       ADD |    - | 16 | 4 | 4 |       MUL |      ADD4 |  4 | 4 | - |   - |    5 |      1 |
|    64 | 64 | 8 | 8 |       MUL |         - |    - |  - | - | - |         - |         - |  - | - | - |   - |    2 |      0 |
|    76 | 12 | 4 | 8 |       ADD |       ADD |   64 |  8 | 4 | 4 |       ADD |       MUL |  8 | 4 | 4 | ADD |    6 |      0 |
|    80 | 16 | 8 | 8 |       ADD |  ADD2MUL3 |    - |  * | 8 | 8 |       ADD |         - |  - | - | - |   - |    4 |      2 |
|    84 | 64 | 8 | 8 |       MUL |       ADD |    - | 16 | 8 | 8 |       ADD |      ADD4 |  4 | 4 | - |   - |    5 |      1 |
|    88 |  8 | 4 | 4 |       ADD |  ADD2MUL3 |    - |  * | 4 | 4 |       ADD |  ADD4MUL5 |  * | 4 | 4 |   - |    6 |      1 |
|    96 |  8 | 4 | 4 |       ADD |       ADD |    - | 12 | 4 | 8 |       ADD |      MUL4 |  4 | 4 | - |   - |    5 |      1 |
|   100 | 64 | 8 | 8 |       MUL |       ADD |    - | 32 | 4 | 8 |       MUL |      ADD4 |  4 | 4 | - |   - |    5 |      1 |
|   108 | 12 | 4 | 8 |       ADD |  ADD2MUL3 |    - |  * | 4 | 8 |       ADD |  ADD4MUL5 |  * | 4 | 4 | ADD |    6 |      1 |
|   128 | 16 | 4 | 4 |      MUL2 |       MUL |    - |  8 | 8 | 1 |       MUL |         - |  - | - | - |   - |    3 |      1 |
|   140 | 12 | 4 | 8 |       ADD |       ADD |  128 | 16 | 8 | 8 |       ADD |       MUL |  8 | 4 | 4 | ADD |    6 |      0 |
|   144 | 12 | 4 | 8 |       ADD |       MUL |    - | 12 | 4 | 8 |       ADD |         - |  - | - | - |   - |    4 |      1 |
|   152 |  8 | 4 | 4 |       ADD |  ADD2MUL3 |    - |  * | 4 | 4 |       ADD |  ADD4MUL5 |  * | 4 | 8 |   - |    6 |      1 |
|   160 |  8 | 4 | 4 |       ADD |       MUL |    - | 12 | 4 | 8 |       ADD |      ADD4 |  4 | 8 | - |   - |    5 |      1 |
|   172 | 12 | 4 | 8 |       ADD |  ADD2MUL3 |    - |  - | 4 | 8 |       ADD |  ADD4MUL5 |  * | 4 | 8 |   - |    6 |      1 |
|   192 | 16 | 8 | 8 |       ADD |       MUL |    - | 12 | 4 | 8 |       ADD |         - |  - | - | - |   - |    4 |      1 |
|   204 | 12 | 4 | 8 |       ADD |       ADD |  192 | 16 | 8 | 8 |       ADD |       MUL | 12 | 4 | 8 | ADD |    6 |      0 |
|   208 | 16 | 8 | 8 |       ADD |  ADD2MUL3 |    - |  * | 8 | 8 |       ADD |  ADD4MUL5 |  * | 4 | 4 |   - |    6 |      1 |
|   224 | 32 | 4 | 8 |       MUL |       ADD |  192 | 16 | 8 | 8 |       ADD |       MUL | 12 | 4 | 8 | ADD |    6 |      0 |
|   256 | 16 | 4 | 4 |       MUL |       MUL |    - | 16 | 4 | 4 |       MUL |         - |  - | - | - |   - |    4 |      1 |
|   268 | 12 | 4 | 8 |       ADD |       ADD |  256 | 16 | 8 | 8 |       ADD |       MUL | 16 | 8 | 8 | ADD |    6 |      0 |
|   288 | 12 | 4 | 8 |       ADD |       MUL |    - | 16 | 8 | 8 |       ADD |      ADD4 |  8 | 8 | - |   - |    5 |      1 |
|   300 | 12 | 4 | 8 |       ADD |  ADD2MUL3 |    - |  * | 4 | 8 |       ADD |  ADD4MUL5 |  * | 8 | 8 |   - |    6 |      2 |
|   336 | 16 | 8 | 8 |       ADD |  ADD2MUL3 |    - |  * | 8 | 8 |       ADD |  ADD4MUL5 |  * | 4 | 8 |   - |    6 |      2 |
|   384 | 16 | 8 | 8 |       ADD |       MUL |    - | 16 | 4 | 4 |       MUL |      ADD4 |  8 | 8 | - |   - |    5 |      1 |
|   512 | 64 | 8 | 8 |       MUL |       ADD |    - | 16 | 8 | 8 |       ADD |      MUL4 |  4 | 4 | - |   - |    5 |      1 |
|   576 | 12 | 4 | 8 |       ADD |       MUL |   48 | 32 | 4 | 8 |       MUL |       ADD | 16 | 4 | 4 | MUL |    6 |      2 |
|   592 | 16 | 8 | 8 |       ADD |  ADD2MUL3 |    - |  * | 8 | 8 |       ADD |  ADD4MUL5 |  * | 8 | 8 |   - |    6 |      1 |
|   640 | 16 | 4 | 4 |       MUL |       MUL |    - | 32 | 4 | 8 |       MUL |      ADD4 |  8 | 8 | - |   - |    5 |      2 |
|   768 | 12 | 4 | 8 |       ADD |       MUL |   64 | 32 | 4 | 8 |       MUL |       ADD | 32 | 4 | 8 | MUL |    6 |      2 |
|  1024 | 32 | 4 | 8 |       MUL |       MUL |    - | 32 | 4 | 8 |       MUL |         - |  - | - | - |   - |    4 |      2 |
|  1088 | 16 | 4 | 4 |      MUL2 |       MUL |    - | 68 | 8 | 8 |       MUL |  ADD4MUL5 |  * | 8 | 4 | MUL |    5 |      3 |
|  1152 | 12 | 4 | 8 |       ADD |       MUL |    - | 16 | 8 | 8 |       ADD |      MUL4 |  8 | 8 | - |   - |    5 |      1 |
|  1280 | 16 | 4 | 4 |       MUL |       MUL |   80 | 16 | 4 | 4 |       MUL |       ADD | 64 | 8 | 8 | MUL |    6 |      2 |
|  1536 | 16 | 8 | 8 |       ADD |       MUL |    - | 16 | 4 | 4 |       MUL |      MUL4 |  8 | 8 | - |   - |    5 |      2 |
|  2048 | 16 | 4 | 4 |       MUL |       MUL |    - | 32 | 4 | 8 |       MUL |      MUL4 |  8 | 8 | - |   - |    5 |      2 |
|  2304 | 32 | 4 | 8 |       MUL |       MUL |    - | 64 | 8 | 8 |       MUL |      ADD4 |  8 | 8 | - |   - |    5 |      2 |
|  4096 | 64 | 8 | 8 |       MUL |       MUL |    - | 64 | 8 | 8 |       MUL |         - |  - | - | - |   - |    4 |      2 |
|  4608 | 64 | 8 | 8 |       MUL |       MUL |    - | 64 | 8 | 8 |       MUL |      ADD4 | 12 | 8 | 4 | ADD |    5 |      2 |
|  8192 | 32 | 4 | 8 |       MUL |       MUL |    - | 64 | 8 | 8 |       MUL |      MUL4 |  8 | 8 | - |   - |    5 |      2 |
| 16384 | 16 | 8 | 8 |       ADD |       MUL | 1024 | 32 | 4 | 8 |       MUL |       MUL | 32 | 4 | 8 | MUL |    6 |      2 |
| 24576 | 12 | 4 | 8 |       ADD |       MUL | 2048 | 32 | 4 | 8 |       MUL |       MUL | 64 | 8 | 8 | MUL |    6 |      2 |
| 32768 | 64 | 8 | 8 |       MUL |       MUL |    - | 64 | 8 | 8 |       MUL |      MUL4 |  8 | 8 | - |   - |    5 |      2 |
| 65536 | 16 | 8 | 8 |       ADD |       MUL | 4096 | 64 | 8 | 8 |       MUL |       MUL | 64 | 8 | 8 | MUL |    6 |      3 |
|131072 | 64 | 8 | 8 |       MUL |       MUL | 2048 | 32 | 4 | 8 |       MUL |       MUL | 64 | 8 | 8 | MUL |    6 |      3 |
|262144 | 64 | 8 | 8 |       MUL |       MUL | 4096 | 64 | 8 | 8 |       MUL |       MUL | 64 | 8 | 8 | MUL |    6 |      3 |

`*` - This column is cannot express a value for the two combined pair of LUTs LUTs in the case where one of them is added to the lower LUTs, and the whole thing multiplied by the other.
