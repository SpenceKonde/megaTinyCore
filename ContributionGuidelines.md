# Contribution Guidelines
Nothing particularly sophicisticated in these guidelines. This is mostly a code style guide.

## Issues are my list of action items
This means that if you believe there is a defect or shortcoming in the core or there is something that I need to do, please create an issue - **NOT a comment on a commit (There's nowhere I can go to get a list of these.** So I see comments on commits in my email, but I get over one page full of email every day, and **it quickly gets lost in the noise, or deleted in a mass delete to keep the Google Drive "you are out of space" reaper off my back.** So if you find and describe a critical bug and write up what is wrong and want me to see it, put it somewhere where I can see it, or you might as well have not written it - I won't see it, and the bug will not be fixed until either I trip over it, or someone else does and reports it. None of us benefit from that.

## Discussion are not a list of action items
They are discussions. I dont check them as often, because I don't expect to see things that I need to do except maybe reply to. So create a discussion only if you suspect your question is a tech support issue that could probably be dealt with by other users, or if you are posting something that is not directed at affecting change in the core (for example, showing off something cool you made with the core (I'd love to add a gallery of links to users blogs/githubs that show off work done with the core), or discussing future generations of parts, etc).

## When Creating issues/etc

### Before creating an issue or discussion topic
Check the existing  [issues](https://github.com/SpenceKonde/DxCore/issues), and recent [discussions](https://github.com/SpenceKonde/DxCore/discussions) and recently closed issues [closed issues](https://github.com/SpenceKonde/DxCore/issues?q=is%3Aissue+is%3Aclosed)
1. If there is a disusssion, read it and see if it applies to you. If it does, either it will solve your problem or you will have additional information that shows that the issue is a defect in the core - if so it is recommended to post that information as a comment in that discussion thread **IFF** there is a large amount of additional and necessary context in the discussion, such that I should be converting the discusssion to an issue. Otherwise reference the discusssion and open a new issue.
2. If there is an existing issue that appears to be the exact same issue, read it and if it really is the same issue, comment there instead of making a new issue - HOWEVER is is *more* important to avoid posting a report of a DIFFERENT issue in an existing issue, as there is no function that lets me split them into separate issues. So this raises the risk that the original problem wiioll; get fixed and the issue closed without your issue being looked at. So don't hijack issues like that.  It is not critical to avoid creating a new issue, since I can always close your issue as a duplicate. If lots of people are tripping over an issue it is helpful to know that it is a problem for more people and
3. If such an issue was **recently** closed, read it to be sure it really is same. Have you updated to a version that I claimed fixed that issue, or if it wasn't a defect in the core, does whatever fixed it for them work for you? If not, reopen the issue. **Note that issues are closed when they are fixed in github, not when a released version including the fix is available** - so you may need to update to the github version of the core in order to test a change.
4. If you don't find such an issue, or it was closed months ago, create a new issue. Reference the old one by number (the number sign followed by the issue number will automatically create a link).
5. If you're inclined to create a discussion: If you're pretty sure it's a defect in the core or documentation (poorly documented behavior, or important points omitted by the documentation), create an issue instead, because **It's easier to turn issues into discussions than the other way around** if I disagree about its being a defect. Discussions are for "How do I do such-and-such?" or "I'm trying to migrate from an 328p to an AVR128DA32, and have some questions", or "My code performs very poorly, how can I make it better/smaller/etc?" and even "Hey guys, I made this awesome thing using one of these parts come and marvel at how cool it is!" (particularly in the last case, also feel free to link to your personal blog or website if you've written about it there! If anyone were to actually submit some such projects, I'd make a page dedicated to it, linking to your site or blog or github (as you prefer), provided that you return the favor by linking to the core - which you should be doing anyway so that people who see it will know where they can get that cool core you used.)

### When creating an issue
* **Include the version of the Arduino IDE** that you are using and a description of the problem, **the version of this core that you are using** and **whether the core was installed through boards manager or manually**. Be sure that:
* Be sure to characterize the issue as completely for best results
  * If you do not get an error (but the code doesn't do what you expect), you must describe how the behavior you saw differs from what you expected to see - "It doesn't work" does not give me enough information to go on. I do not know what correct and incorrect results in your projects look like. If the program is small or the problem is readily localized to a certain area of the code, such that I might be able to figure it out from the asm,, do "Sketch->Export compiled binary", "Sketch -> show sketch folder" and zip up the most recent .hex, .lst, and .map files and attach them
  * If a compile or upload error is generated, do File -> Preferences, and check show verbose output during "compilation" or "upload" as appropriate. Re-attempt the failed attempt, and far more console output will be generated. Place this output in the issue within a code block (on a new line, place a row of three backticks, then on the following line, paste in the block of verbose output, then on the following line, anotther row of backticks.
  * If it is an upload error,  be sure you get the first line of upload output, which will contain the command it is trying to use to upload. *This is almost always the most important part of verbose error output* yet I almost *always* have to ask for it - I believe this is because the copy error messages button of the IDE does not copy it. Yes, that's right the copy error message button misses the most important line of output.
2. **If reporting a compile error** please perform these two very simple tests:
  a. try to compile bare minimum for the same board and settings. If this fails, the core is not installed correctly, or there is a larger system-level issue. Perform the next test in that case:
  b. **If that fails**, try to compile bare minimum for the Arduino Uno. If *that* does not work either, something is wrong with your Arduino IDE installation in general, and you should post in the Arduino technical support forum, here.
3. **If the problem involves a library not included with the core, always include the URL that you got the library from, or if installed from Library Manager, the version**. Otherwise, the issue is not actionable and you will be immediately asked for that information and no work on the issue can occur until it is provided. That is because the name of a library **does not uniquely identify the library** - Frequently there are **hundreds** of forks of popular libraries all over the web, with the same name and almost always an identical readme and version number to the upstream repo at the time it was forked, but with (rarely documented) changes ranging from trivial to major. I have no way of knowing what version you are using. Do check the supported libraries page (shared by DxCore and megatinyCore due to the great deal of similarity between them) to make sure that it is not known to be a defect in the library, and that there is no working version of the library (there is one library, for example, that has steadfastly refused to support any post-2016 AVRs, even when given PRs that will fix it for all current and announced products, in that case, the fixed version that was used for the PRs is linked to from that page.)
4. **For any error involving a library, check if the problem reproduces with any of the examples included with that library.** If so, that is **HUGE** as it proves that the problem is **not a bug in your code**, though the inverse is not true. A bug in the library may not trigger any failures in the usually cursory examples.
5. If you have fixed a defect in the core, you are encouraged to submit a pull request. This way you'll get official credit and prestige through github - Why do you care? Well, presumably you're going to at some point be looking for a (new) job. Future employers will look at your github profile! Opening PRs and issues, discussing solutions to them, and so on will demonstrate that you know how open source development and source control work, and that's a skill that can't hurt your in the job application. It is said that in some circles, your github profile is as funtamental as your resume.

### When creating a pull request
1. If you're thinking of implementing a new feature, that is wonderful - but please create a new issue first. Several times new features have been implemented at the same time that I was implementing the same feature. which is awkward for both of us. I also may have additional information that is relevant to such an endeavour; for example, I may have tried to implement it, discovered that it wouldn't work, and tracked the problem back to a silicon bug, or implemented it successfully, only to realize that it had negative consequences for other aspects if functionality, concluded that it was not possible to implement without unacceptable consequences. I could tell you what approach I used that had that problem, and if you can see that your plan would run into the same issue, you might call off spending time on this, or you may find some clever way to get around them.
2. After creating a pull request, CI tests will be run automatically (note that I have to 'allow' this the first time it's done for any user) If errors are turned up in the CI testing, make sure your code didn't cause them. We can't merge code that breaks stuff. It gets run through codespell, formatting checks (using astyle), and compile-tests.
  a. All markdown files get checked with an extremely pedantic markdown checker called MarkdownLint.
  b. For the case of **"Check Code Formatting", it may give spurious complaints, almost always about indentation**. This is because it directly conflicts with the Azduino Code Style Guidelines. That's why the core itself doesn't get check codee formatting run on it, only libraries, examples, and variants. If fixing those formatting complaints would make the code harder to read or conflict with the Azduino style guidelines, readability trumps compliance with any guidelines, and the Azduino style guidelines trump astyle, except in trivial cases. You can wrap short sections it doesn'd like in a pair of single line comments `// *INDENT OFF*` and `// *INDENT ON*` or if the whole file generates tons of style issues that you know are spurious (something with lots of inline assembly, for example) you can just turn off indent checking at the start of the file (and not turn it back on).

  c. **The most important** - if Compile Examples fails, click actions, and view compile examples for the last run on master, and the run on your PR, and make sure that if there are any new failures, they are spurious (errors that list http errors when downloading files are in this category - even with an authentication key, we are getting ratelimited sometimes)

4. Try to follow the Azduino code format style (see below), which is a series of conventions above and beyond (and in some cases, in conflict with) astyle - this means less work for me to merge your fix.

5. astyle is not run on the core itself, only on libraries and examples. This is because the features of astyle that are not tunable would mean I'd have to disable astyle on almost every file anyway (astyle doesn't like indentation for nested preprocessor conditionals. The code is hopelessly unreadable without indenting these).

## Design decisions that some may disagree with are made for this core
Rational people may disagree with some of the design decisions that I made for this core, where the core does not behave the same way as the official core (these are listed in the readme, near the bottom). These are not bugs, these are features, and for me to change these, you need to make a very convincing argument that my design decision was bad. Irrational people may also disagree with design decisions made in this core (in fact, thus far the majority of objections have been of this sort). T
Some general themes of these decisions others may disagree with:
* ArduinoAPI was a disaster for AVRs, Where ArduinoAPI files are altered, bypassed or removed, that was intentional. If it causes problems, and you don't know a way of fixing it, create an issue so we can discuss how to resolve it and restore compatibility without also bringing back whatever problem I was trying to get rid of.
* Be very careful with flash usage, particularly for code that could be used on tinyAVRs! Large amounts of code are shared between DxCore and megaTinyCore. While code that adds 200 bytes to the size of compiled files (you can see this in the CI results) may seem innocuous on an AVR128Dx-series part, when that gets ported to megaTinyCore, it will represent 10% of the total flash available on some parts! With future Ex-series parts dropping as low as 8k flash, DxCore must treat flash space like we do for tinyAVRs.
  * Shared code includes Wire, SPI, Logic, Event, Comparator, Servo, EEPROM, tinyNeoPixel, USERSIG (only some parts), and SoftwareSerial.
    * Flash on DxCore and megaTinyCore have the same name and purpose, but a completely different API (the underlying NVMCTRL hardware and vision of the library authors is dratically different).
  * Large portions of the core itself are also shared, done by periodic directory-diffs with each difference manually reviewed. and either the newer version merged, or the   two versions recognized as needing to be distinct due to the differences in the hardware.
* See [Differences between DxCore and the official cores](https://github.com/SpenceKonde/DxCore#differences-in-behavior-between-megatinycore-and-official-cores) in the Readme. These changes are intended. If you don't like them, open an issue and convince me that it should not be the way it is, PR's will not be merged that undo intetional design decisions without changing my intent first.

## Azduino Code Style
We have a largely self-consistent code style used throughout the core and associated files based on a number of principles.

1. Readability is paramount, and if adhering to particular code styling guidelines would result in hideous looking unreadable code, don't do it.
  a. astyle is only run in libraries and variants, not the core itself because it would need to be disabled on all files anyway.
2. Always use the integer types that explicitly specify the datatype size. That means, avoid 'int' 'byte' 'char' 'long' etc. Types should be things like:

| size   | signed  | unsigned | grudging | avoid | avoid | avoid        | Notes |
|--------|---------|----------|----------|-------|-------|--------------|-------|
| 8-bit  |  int8_t |  uint8_t |        . | short |  byte |            . | Use uint8_t's whenever possible |
| 16-bit | int16_t | uint16_t |   size_t |   int |  word | unsigned int | C likes to default things to int16_t - this is often undesirable. |
| 32-bit | int32_t | uint32_t |        . |  long |unsigned long         | Use 32-bit datatypes only when needed |
| 32-bit |   float |      n/a |        . | double|     . |            . | Avoid 'bloating' point values unless absolutely necessary. |
| 64-bit | int64_t | uint64_t |        . |  long long | unsigned<br/>long long | . | 64 bit datatypes are not used by the core in any way shape or form |

  a. Always use the smallest datatype appropriate for the data.
  b. If either a signed or unsigned value will work, use the unsigned one unless there is a reason not to.

3. If constants are used as parameters or return values, they should be #defined and in all caps.
```c
#define FOO_BARRED (0)
#define FOO_NOT_BARRED (1)
#define FOO_CANNOT_BE_BARRED (-1)
```
4. function names: camelCase. This includes function-like macros
5. "hidden" things, like a special functions and methods not meant for use by end users should be start with a _
6. For readability, please observe these style patterns. See comments below.
```c
uint8_t foo() { // space after the close paren, open bracket on same line. Space betweenn { and //, and between // and the comment.
  uint8_t temp = getValue();  // space before and after =.
  if (temp == 0) {  // again, space before and after operators, except as noted below
    bar();
    return FOO_BARRED;
  } else if (temp == 255) { // the `} else if (condition) {` should be on one line in if/else if statements
    return FOO_CANNOT_BE_BARRED;
  }
  return FOO_NOT_BARRED
}
```
7. Single line if-then statements (with no { }'s ) are discouraged.
```c
  if (temp == 255) return FOO_CANNOT_BE_BARRED;
  // or
  if (digitalReadFast(mypin) == HIGH) return -1;
```
  This sort of construction is acceptable in the core only (not libraries, examples, or variants), and even then it is deprecated and allowed **if and only if** the added line, braces, and indentations actually would make it it less readable instead of more. Typically *this is only the case when the conditionally executed code is a **single trivial** statement* - a return statement, incrementing a variable, that sort of thing.

8. Braces do not get their own lines

This construction is **not** acceptable. It needlessly inflates the visual size of the code so you can't see as much on the screen and does not benefit readability in any other way. Since readability is better when you can see more of the code, all else being equal, this fails the readability requirement.
```c
  if (condition)
  {
    ... // code
  }
  else
  {
    ... // code
  }
```

Look how much less visual space is taken up when the coding conventions are followed:

```c
  if (condition) {
    ... // code
  } else {
    ... // code
  }
```

**Note that the bit about spaces between operators and operands does not apply to the unary `*` and `&` operators!**
```c
PORT_t *portptr = &PORTA; // good
PORT_t * portptr = & PORTA; // no. These look too much like multiplication and a bitwise and that's missing the first argument.
```
This is down to personal taste, and is the sort of thing crusades are waged over. In my mind the * is not acting as an operator but rather part of the symbol (if var is a variable, &var is the address it is located at) as opposed to the "correct" interpretation (the & operator takes a variable as argument and returns it's address).


7. **INDENTATION**
  a. 2 spaces per indentation level. All conditionally executed or compiled code MUST be indented.
  b. Spaces, not tabs. There are to be no tabs in any file other than keywords.txt (which requires them).
  c. Exception to 2 spaces on all indentation: no extra level of indentation for the contents of a header guarded by an includeguard.
  d. When there's a large block of conditionally compiled code or a large block in an if/etc (ie, if you can't see the whole thing on the screen at once, the end should get a comment indicating which block it's the end of. This is optional but encouraged elsewhere
  e. In headers and other places where there is a list of function prototypes or a run of similar bits of code, it is encouraged to pad the space between extra spaces to line them up and improve readability. Like thisd:

```c
uint8_t  foobar = 1
uint16_t foobaz = 65535
uint8_t  foo    = 1
// more readable than:
uint8_t foobar = 1
uint16_t foobaz = 65535
uint8_t foo = 1
```
  e. This particularly applies to constant tables, and especially where you can make them more clear to a reader by using odd indentation :
```c
const uint8_t constvalues[33] = {
  255,   0,   1, 255,   2, 255, 255, 255,   3, // First 4 bitmasks can be converted to bitpositions with a 9 entry table
       255, 255, 255, 255, 255, 255, 255,   4, // 17 entries gets us the 5th
       255, 255, 255, 255, 255, 255, 255, 255,
       255, 255, 255, 255, 255, 255, 255,   5, // and 33 entries gets us the 6th.
}
```
This will require defanging astyle as described above on files that aren't part of the core.
Notice how in this case, we noticed that we had an unusual structure for the table of 33 entries: values 8, 16, and 32 are special, and would naturally go on the end of the line, but the first line starts at 0,

8. Inline assembly is subject to some special guidelines:
  a. The `__asm__ __volatile__(` statement should be on it's own line except for single instruction inline asm routines.
  b. We need 2 levels of indentation (minimum) for all lines containing instructions. Lines with a label get 1 level indentation less.
  c. Only one label or an instruction should go on any line.
  d. Assembly is hard to read. Almost every line should have a comment
  e. Format it so that the operands to each instruction are lined up in columns
  f. Line up all the "\n\t" sequences.
  g. unless your asm takes only a small number of output or input operands, put input and output on separate lines. If you think it makes it clearer to put each opperand on it'd own line, do that.
  h. You can't rjmp to a label in another function. You can jmp to it (though this only works because of LTO and technically is not above board), but not rjmp. Just because you have an rjmp doesn't mean the linker will decide to place the routine you want to jump to within range of an rjmp! Don't forget also that you can't jmp on parts with 8k of flash or less (with a small number of oddballs, like the m808/809
  i. Your code must be correct and valid: A pointer to a volatile variable, which you use to change that variable using st X/Y/Z or std Y+q/Z+q only is an input operand. You haven't changed it! You've changed what it's pointing to, but you haven't changed the pointer. Similarly, you must never use a pointer to a non-volatile variable to write to that variable from within asm. The only variables you are can change are volatile variables accessed via pointers, and opperands declared with the read-write or write only constraint.
  j. A pointer to a variable, even if you only use it to read the variable, if you use ld X+/Y+/Z+ or ld -X/-Y/-Z must be marked as a read-write operand - the operand is the pointer, and you're changing it!
  k. If you specify a write only constraint it may contain anything until you write it. Specify a read-write opperand if you need to, for example, add to an existing value.
  l. Triple check your constraints. The problem with wrong constraints is that they sometimes compile... only to not compile under other circumstances. If you use SBIW/ADIW on something, it must have the w, e, x, y, or z constraint. This will often go unnoticed until it gets inlined into someplace where there is high register pressure, at which point it will no longer compile. If you use any of the 'immediate' instructions, the register being operated on MUST be specified with the +d or =d constraint (to get an upper register which you're allowed to write to). Again, this will not cause an error until the compiler happens to assign a register that is not an upper register, and which point everything promptly stops compiling with a very confusing error message. Wrong constraints are insidious for this reason
  m. Inline assembly is hard to read, and hard to maintain. Don't use it gratuitously or where the benefits are negligible. Use it only where execution speed really really matters - that's why for instance, in these cores, you will find it in timekeeping functions, in ISRs, and not really elsewhere.
  n. If it is outside of the core, you *will* need turn off astyle indentation checking or it will fail style checks.

This is an example of how it should look
```c
uint8_t myfunc(uint8_t a, uint8_t b) {
  /* Returns b + a + (a-1)... or 255 if that would exceed 255
   * Note that this is not a justified use on inline asm unless the code is time-critical, AND you compiled C code that does the same thing checked the assembly listing
   * and found the generated asm to be pathologically inefficient.
   */
  if ((!b) || (!a)) return 0; // don't proceed if a or b is 0, function is documented to require a non-zero value here.
  uint8_t retval;             // we need a variable to stuff the result into. Do the same for scratch variables.
  __asm__ __volatile__(       // No ASM on the same line as `__asm__ __volatile__(` unless there is only a single instruction
      "mov    %0, %2"                   "\n\t" // Copy b to the return value - notice that we add at least 2 indents, not just 1.
    "_myfuncloopstart:"                 "\n\t" // Labels should have 1 less level of indentation than everything else
      "add    %0, %1"                   "\n\t" // And every line (almost) should have a comment
      "brcs   _myfuncend"               "\n\t" // Notice how the labels contain the name of the function to prevent name conflicts.
      "dec    %1"                       "\n\t" // decrement a.
      "brneq  _myfuncloopstart"         "\n\t" // if we haven't hit zero yet, return to start of loop. Notice also that as an internal function, labels are prefixed with _.
      "rjmp   .+2"                      "\n\t" // skip next instruction
    "_myfuncend:"                       "\n\t" // here's where we jump at the end if we overflowed in the loop...
      "ldi    %0, 255"                  "\n\t" // This is the instruction that we are rjmp'ing over if a reached 0 without an overflow. Otherwise we jumped to _mufuncend and
                                               // perform the ldi
    :"=d" ((uint8_t) retval),                  // Output operands and input operands should go on different lines, if both kinds are present
     "+r" ((uint8_t) a)                        // and it is acceptable and often desirable to put each operand on it's own line entirely.
    :"r" ((uint8_t) b)                         // I have no position on named variables vs not naming them - both are acceptable.
    : /* no clobbers */                        // It is optional to state explicitly the absence of one kind of constraint or another.
  );
}
```

9. Really really ugly #ifdef hell is undesirable. yet often we have no choice; Alternate indentation schemes may be appropriate for this. See event.h for one such example.

## Things to be aware of when writing PR's

### Help us prevent bad code from getting merged
Do not submit pull requests containing bad code.

### Avoid common mistakes
* macro definitions must have parentheses around the value they will take.
* Be sure to consider what happens when millis is disabled. **This is the second most commonly chosen option for millis timekeeping after the default!**
  * delay() and delayMicroseconds() both work with millis disabled (in the case of delay, only if millis is actually disabled from the menu, not enabled and subsequently broken by user code).
* macros DO NOT CHECK TYPES; your macros that are exposed to user code library code must be hardened to take stupid types passed to them. Remember that while the core uses uint8_t's for all pin identifiers, code in the wild frequently follows the boneheaded lead of the Arduino IDE and uses signed 16-bit integers. A bug discovered in early 2023 involved the fact that if certain pin information macros were called with a signed value, the math where we checked if it was a valid pin could fuck up; we were checking if pin >= NUM_DIGITAL_PINS. When the values are unsigned, this works because -1 = 255 > pin whenever pin is valid - a single test can be used to catch both known NOT_A_PIN and other invalid pins not yet identified as much. But if a signed integer was passed to the macro, the default signed integer type would remain in use for the comparison with 0. So any negative number would pass through the test, and down the line would cause us to read from a memory location that was not part of the table we were looking values up in. If there is anything type sensitive, explicitly cast it.
* Because of this, in macros, always use 255 if for some reason you have to write out the value of the NOT_A_whateveritis error values, not -1, even though you know it will end up in a uint8_t. There's no advantage to -1, but though you may know what type it will eventually have, you do not know the path it will take to get there, which may involve multiple data types.

### Performance and binary size matter.
Particularly within the core API, performance matters. That means that things like division and modulo are bad, especially on large datatypes. Division and modulo both run the same routines. Approx time cost of math (how the variables are initialized may have an impact of +/- 2 x (size of datatype), so this is not gospel. The time required for division shown below is an average, since that routine does not execute in a fixed period of time. Large numerators and small denominators make it worse - as you would expect, computing x / y where x < y is highly favorable. Notice that switching to 64-bit types from 32-bit ones more than doubles the execution time - this is due to register pressure. It is likely to actually have an even larger effect in practice if there are other variables that need to be swapped out of working registers and reloaded.*

| Math Operation | uint8_t | uint16_t | uint32_t | uint64_t | float |
|----------------|---------|----------|----------|----------|-------|
| Addition       |       5 |       14 |       20 |       54 |   116 |
| Multiplication |       8 |       22 |       81 |      371 |   141 |
| Division       |     233 |      235 |      604 |     1734 |   479 |

The obscenely slow division of 32-bit datatypes is why micros has all that ugly assembly in it - division was way too slow. So we used a combination of bitshifts and addition, and used tricks to minimize the size of the operands at all times. But the compiler rendered it as an incredibly inefficient mess. (a + (a >> 1) + (a >> 2)) was rendered as (copy a to a temp register. Then copy it again, rightshift that once, add to temp register. Copy the original, replacing that singly rightshifted version. Rightshift twice, and add). Instead of simply rightshifting the intermediate value another place and repeating the addition.

If you have to use this "ersatz division", there are two things to remember. First, a - (a >> 2) - (a >> 3) is almost the same as a - (a >> 1) + (a >> 2) - (a >> 3). Except that integer math and rounding can bite us in the arse here. For maximum accuracy, organize the terms so that when the terms are in order from largest to smallest, the sign alternates. In the above example the second case is more accurate, assuming the theoretical goal is to find `0.625*a`, though it is slower. But wait - you can then perform the same substitution in reverse on the second version, yielding a better one: a - (a >> 1) + (a >> 3), however trying to improve the performance of that by turning it into (a >> 1) + (a >> 3) will lower accuracy, because you have two positive terms in succession (if you imagine an implicit `0 +` in front of those, this may make more sense)  You will of course want to balance the cost of adding more terms against the required accuracy.

Sometimes codesize and performance are in opposition. How do you know which is more important? A good rule of thumb is that if the overhead is introduced only when people use the specific feature, which is not a feature that is so widely used in libraries and sketches in the wild that it will impact most code anyway, sacrificing size for speed is relatively more acceptable. The only people who will get the overhead are those using the feature that you've added or made better in some other way. If the overhead is imposed on everyone (because it's located in initialization code, or in something so widely used that it might as well be) code size becomes much more important. Seriously, people have made angry issues over 20 bytes, and others have complained about the overhead of the core itself, citing how large bare minimum was, comparing it to an empty C program compiled with microchip studio and declaring that the whole core was garbage because look how much flash it throws away to do nothing. Of course it doesn't do nothing - even bare minimum is doing a bunch of stuff - setting up the ADC so analogRead works setting up the timers so analogWrite works, initializing tables so that pin numbers can be used `*`, catching dirty resets to prevent erratic and unpredictable misbehavior if you smash the stack or trigger an ISR that doesn't exist, and making the CPU run at the speed you asked for, and (if not disabled), setting up millis timekeeping). My point is that people, particularly on the 8-pin tinyAVRs which come only in 2 and 4k versions, will scream bloody murder about small increases in globally applicable overhead, so that must be avoided

### Constants in PROGMEM and different cores
Code is constantly ported between DxCore and megaTinyCore. You need to be aware of one particularly important difference between parts with 32k of flash or less and those with more. With 32k of flash or less, all flash is mapped, and you do not have to (and should not) use the PROGMEM keyword for a table of constants which you want to stay in flash - the compiler will automatically know to leave the table in flash, and it can be accessed normally. But when the flash is larger, the compiler cannot do that, because only one section of the flash (defaulting to the highest one) is mapped at any given time, and that can be changed (though it rarely is). There are three ways to deal with this:
* Use PROGMEM keyword and pgm_read_x_near() everywhere. This initially looks to be of equal performance - lpm is 3 clocks, ld is 2, but you have to add one because it's talking to NVM. But in fact, it is slower in any case more complicated than that; for example, consider looping over a 10 element array, reading each value in sequence. If it's located in memory mapped flash, it will likely be accessed with ld X+ or ld Z+, taking 3 clocks per byte. If it's located in PROGMEM and accessed using pgm_read_byte_near(), although there is an lpm Z+ instruction, pgm_read_byte_near() can't use it - pgm_read_byte_near() is just a macro that gets transformed into a tidbit of asm that executes lpm Z. So it would turn into `ld rxx, Z; adiw r30, 1 ld rxy, Z; adiw r30, 1;` For n bytes read like that, it will take 3n + 2(n-1) or 5n-2 clocks, instead of 3n clocks (not counting the time to set the pointer up, which is the same for both methods). Or consider a one-off access to a byte; if it's in mapped flash, this can be done in 4 clock cycles and two words - the read gets rendered as lds, which runs in 3 clocks + 1 because nvm. If it's in PROGMEM, and is being accessed with pgm_read_byte_near() the address must be loaded into the Z register (2 words 2 clocks - or up to 6 words and 8 clocks if Z is already in use, counting cleanup), and then lpm executed (3 clocks, 1 word). For a total of 5-11 clocks and 3-8 words vs 4 clocks 2 words. See why the mapped flash is a big deal now?
* **Not permitted, will not be merged in this form** - Declare the array as PROGMEM_MAPPED. This places it in the mapped section of flash **provided the user does not change what that is**. If they do, they'll get garbage from elsewhere in the flash when they try to read their table, hence this isn't acceptable in the core. Application code is welcome to do this, but only because in that case the user is in control of their code, and if they are using PROGMEM_MAPPED, and changing FLMAP, they deserve what they get, and have violated an explicitly documented prohibition (see Ref_PROGMEM.md). Currently tinyNeoPixel does this on DxCore, but that will not be the case after a planned enhancement for DxCore is implemented. This enhancement will do the following:
  * Add a menu to tools that will default to "No", allowing to specify whether changing FLMAP is permitted.
  * If "No", we will lock the FLMAP bits before any user code runs, and #define PROGMEM_MAPPED to direct code to that section of flash, and #define FLMAP_LOCKED.
  * If "Yes", we will not lock the FLMAP bits. PROGMEM_MAPPED will not be defined.
  * There may one or more additional options - "No, but set FLMAP to section `__`" - the only time I see this being useful is on the 128k parts when address-constrained spm-from-app is enabled, and you want the memory mapped section to NOT overlap with where you can write to from the app (usually you want that overlap). This will also define PROGMEM_MAPPED appropriately and FLMAP_LOCKED.
* A solution that will be sound now and acceptable in the core or libraries without compatibility issues thus is:
  * If `__AVR_ARCH__` == 103, then the part has fully memory mapped flash. Just declare the variable const and access normally, it won't be copied to ram like it would be where `__AVR_ARCH__` != 103.
  * If FLMAP_LOCKED is defined, this change is implemented, and you are on a part has only partially mapped flash, but the user has not asked for FLMAP to be mutable, so it will be locked while app code is running. PROGMEM_MAPPED will be defined, and you can declare with PROGMEM_MAPPED and access them without the pgm_... macros (remember that this will result in flash with a big hole in the middle, and may cause programs to return inaccurate flash sizes during compilation and upload).
  * If PROGMEM_MAPPED is defined but FLMAP_LOCKED is not, this change has not been implemented yet, so FLMAP is mutable. You cannot safely use PROGMEM mapped in core or library code - that code could be used by people who change FLMAP, and it would then break horribly; you must declare the array PROGMEM and access with pgm_... macros. Note that this does not apply to your application, because there you control the application, and if you use PROGMEM_MAPPED and change FLMAP, you will get exactly what you deserve - severe breakage.
  * If neither PROGMEM_MAPPED not FLMAP_LOCKED are defined, then either it's a classic AVR, or it's a modern AVR where `__AVR_ARCH__` != 103, and the user has told us that they intend to change FLMAP. Either way, you need to do the same thing - PROGMEM and pgm_... macros.
* Any code going into either core (unless it is specific to tinyAVR only and hence we know it won't be ported, and I agree with you about that), as core or library - must either use only PROGMEM and pgm_... macros, or must handle all cases appropriately using #ifs to conditionally compile the appropriate version, as shown below.

```c
#if __AVR_ARCH__ == 103
  const uint8_t constanttable[]={0, 1, 2, 3 ,4};
  #define lookupconstant(i) (constanttable[i])
#elif defined(FLMAP_LOCKED)
  const uint8_t PROGMEM_MAPPED constanttable[] = {0, 1, 2, 3, 4};
  #define lookupconstant(i) (constanttable[i])
#else
  const uint8_t PROGMEM constanttable[] = {0, 1, 2, 3, 4};
  #define lookupconstant(i) (pgm_read_byte_near(&constanttable[i]))
#endif
```
