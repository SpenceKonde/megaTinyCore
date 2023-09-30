# CCL Toolbox

There are some simple patterns that you can use with the CCL/Logic library to generate very widly applicable effects:

In the below examples X, Y, Z and 2 are used to refer to inputs. X, Y, and Z can refer to any input, but for the purposes of the LUT presented, X = 0, Y = 1, Z = 2

Input 2, since it can be used as a clock, may be specifically required.

## Feedback
The "Feedback" channel is the **output of the sequencer if used, and the even LUT in that pair if not**,

### But I need feedback on an ODD LUT
"Ya can't get there from here". Well, you can, but it'll cost you. The price is one event channel: Use the output as a generator, and set one of the event inputs of the CCL block to that channel

## A bit more on clocks and timing
The clocks have some counterintuitive behavior. First off, what do they and do they not effect? The clocks are used by:
* The Filter or Synchronizer, if enabled.
* The sequencer, if it is configured as a flip-flop.
* The Edge Detector
It does not govern the speed of the response otherwise - the CCL reacts asynchronously (it is not clocked, and happens as fast as the silicon can manage) unless the edge detector, synchronizer, filter, or sequencer are enabled.

### The edge detector
Sometimes you need a pulse when all you have is a level. This gets you there. The clock is involved because the resulting pulse is 1 CCL clock long (occasionally this is not long enough, since the CCL clock can be faster than the system clock, particularly on the EB, where you can clock the CCL from the PLL, or you may be using a very slow clock, and it could be troublesome how long it is.

### The ~programmable delay~ synchronizer/filter
This is one of the really cool, repurposable features. The intended use is that you can use the synchronizer to take a 2 clock cycle delay to ensure clean transitions and prevent glitches, with the filter meant to provide a means of cleanly handling more substantial noise by requiring that the signal be unchanged for 4 CCL clocks before outputting it. One nuisance is that according to the datasheet, one of these settings must be enabled to use the synchronizer.

### The sequencer
The sequencer, in some modes, also uses the clock. It takes the clock from the EVEN LUT when acting as a flip-flop (but not when acting as a latch).

### Ideas for application
Imagine you wanted a slightly longer pulse from a level change - say, 2 or 4 clocks clocks - than edge detector can get you. So you take a pair of LUTs, and enable the sequencer, with an R/S latch selected. Configure both of them identically. The one on the S line gets the synchronizer, and the one on the R line gets the filter. Result? When the inputs match the truth table, the signal will make it's way through the 2 stages of the synchronizer on both LUTs. This will set the sequencer output. Two clocks after that, the signal will make it through the second LUT and reset the sequencer.

Or you can skip the sequencer, run the higher numbered LUT in level mode to interpret the inputs, and the adjacent one to make a pulse. You'd get 2 or 4 clock long pulse from it - but **that clock can be a different clock from the first one**.

### The signal proceeds through a logic block's subcomponents in an order
It's important to understand the signal path.
1. The LUT - asynchronously monitors it's inputs and outputs a value based on the truth table.
2. The Filter/Synchronizer - Takes input from the LUT. Either does nothing, performs a 2 clock synchronization, or performs synchronization, then feeds that through 2 additional flipflops, and compares the output of the last one with the sync output, and only changes if the two match, indicating a signal stable for more than 2 clocks. This introduces a delay of 2 or 4 clocks
3. The edgedetector, if enabled, takes the output from the filter/synchronizer, and reacts to edges in that (note that per datasheet, you need to have one of those two enabled - unclear if that's "Or else" or "Because we put an interlock to prevent you from doing that"). That pulse starts as soon as the signal gets in, and lasts one clock.
4. Finally, if the sequencer is enabled, it overrides the output of the even LUT.
5. This signal - the sequencer if enabled / Even LUT's output if not - is what feeds both the pin (if enabled) and the signal that is provided as feedback (if used).

Thus - the LUT comes first. It's output then may or may not be filtered/synchronized and if it is, may be edge-detected. If we imagine a LUT taking say, the OR of 2 inputs, and put a filter on it, and each of those inputs is (for simplicity) changing according to the same clock as the LUT. If the two inputs are, repeating: HLL and LHL, the output from the first stage will be HHL. The filter will then turn that into *a continuous high level*.

## PWM is magic
Unlike the event channels, where you get a single clock long pulse from a compare match or overflow, the inputs to logic blocks are the level of the output compare! That means that you can remap pins that don't exist on your part but would have PWM if they did to the LUT output pin.

### Long-armed PWM
Reach out and use a timer on a distant pin instead of the (otherwise occupied or non-existent) one it would normally get.

INSEL:
```text
X: TCA WO0, TCB0 or TCD WOA
Y: masked
Z: masked
or
X: masked
Y: TCA WO1, TCB1 or TCD WOB
Z: masked
or
X: masked
Y: masked
Z: TCA WO2, TCB2 or TCD WOC (which is WOA)
```

LUT - Only one input is used, so there will be 2 bits that matter and 6 that don't (and should be left 0 by convention), one will be 0 (the low bit - you want ) and the other 1. So for non-inverted PWM, this is just
* 0x02 (0b00000010)
* 0x04 (0b00000100)
* 0x10 (0b00010000)

Clock: N/A

Sync/Filter: Off

Note that TCA WO3-5, TCB3+, and TCD WOD are not available as inputs.

### Out-of-phase PWM
Problem: You have 3 output channels, each of which requires PWM at a different duty cycle (total not exceeding 100%), but they control large loads, and your power supply can only power one at a time.

If you use a TCA (remembering to call the takeover function), you can run it in in SINGLE mode, getting 16 bits of resolution.

If you then used it normally, the three duty cycles would be written to CMP0, CMP1, and CMP2, but they would all turn on at once, that's not okay. Instead, you could set them as:
* CMP0 = DS0 - 1 -> Output on the pin by setting pin output and writing 1 to the CMP0 bit
* CMP1 = DS0 + DS1 -1 -> Output through the CCL
* CMP2 = PER - DS3 -> Output on the pin by setting pin output, setting INVEN (inverting the pin), writing 1 to the CMP2 bit.

CMP0 < CMP1 - Ensured by above, provided that:

DS0 + DS1 <= PER - the duty cycles of the first two pins must not exceed 100%

To meet the specific specification (one at a time only) we would need to be sure that DS0 + DS1 + DS2 <= PER, but not all applications require that of the final duty cycle (maybe there are only two big loads, and one light load... (possibly a cooling fan on the power supply?)

INSEL:
* X: WO0
* Y: WO1
* Z: masked

LUT:
* 000: 0 - During the time before CMP0 is reached, the second big load shouldn't be on.
* 001: 1 - Now CMP0 has been reached but CMP1 hasn't, so here is where we want the to turn this output, controlling the second load.
* 010: 0 - This is never reachable in practice if CMP1 > CMP0.
* 011: 0 - Once CMP1 and CMP0 have been passed, we turn off WO

Clock: N/A

Sync/Filter: Off

Yes, we could have done the third one with another CCL lut too - but why when there's a trick to do it without wasting a second LUT? Maybe we need the other LUTs.

### Modulated PWM
Like classic AVRs had on larger pincount devices. One PWM frequency should be significantly higher than the other if you're trying to modulate it, rather than measure the beat frequency or something, and they definitley should be at different frequencies, otherwise see the previous pattern.

INSEL:
* X: Timer PWM channel.
* Y: Second PWM channel.
* Z: masked

LUT: 0x08 (0b00001000, HIGH if both inputs are high)

Clock: N/A

Sync/Filter: Off

###

## Sequential logic with just one LUT
You can simulate some sequential logic units with just one LUT!

Enable the synchronizer to get the analogous flip-flop.

### S-R latch

INSEL:
* X: Feedback
* Y: Set (any input source)
* Z: Clear (any input source)

LUT:
* 000: 0
* 001: 1
* 010: 1
* 011: 1
* 100: 0
* 101: 0
* 110: Per application requirements - logic block is getting contradictroy signals
* 111: Per application requirements - logic block is getting contradictroy signals
Ergo: TRUTH = 0x0b??001110 = 0x07 (go low when told to go both directions), 0xC7 (go high when...) or 0x47 (don't change when...). Avoid 0x87 (Oscillate rapidly at an unpredictable speed when...)

Clock: N/A for latch, anything except IN2 as clock as demanded by application for flipflop.

Sync/Filter: Off for latch, on for flip-flop.

### D-Type latch

INSEL:
* X: Feedback
* Y: D (gated signal to latch)
* Z: G (Gate)

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

Clock: N/A for latch, anything except IN2 as clock as demanded by application for flipflop.

Sync/Filter: Off for latch, on for flip-flop.

## More patterns

### A/B select
Allows selection of one of two inputs as it's output based on the remaining input. Obviously, chainable if you have to.

INSEL:
* X: Selector
* Y: A - output when selector low
* Z: B - output when selector high

LUT:
* 000: 0
* 001: 0
* 010: 1
* 011: 0
* 100: 0
* 101: 1
* 110: 1
* 111: 1

Ergo: TRUTH = 0xE4

### Gated Buffer
INSEL:
* X: D - When G is high, D is output
* Y: G - When G is low, the output is low.
* Z: masked

LUT:
* 000: 0
* 001: 0
* 010: 0
* 011: 1
Ergo: TRUTH = 0x08

Clock: N/A

Sync/Filter: Off

### Double-gated Buffer
There are many variants on this where different combinations of logic are used. The point is to get an "If A and B, output C, else output (whatever level it is"

INSEL:
* X: D - When G is high, D is output
* Y: G - When G1 is low, the output is low.
* Z: /G - When G is high, the output is low.

LUT:
* 000: 0
* 001: 0
* 010: 0
* 011: 1
* 100: 0
* 101: 0
* 110: 0
* 111: 0
Ergo: TRUTH = 0x08

Clock: N/A

Sync/Filter: Off

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
