# Linking against static library

In some uses cases, it might be convenient to link against a static library/archive rather than compiling the code each time. For example if you don't have access to the source files, just the headers and the archive, you want to reduce compilation time for a larger codebase or want to have a bunch of board specific code for a larger library. The downside with using a library is that the code size will increase, since things are statically linked and the compiler can do fewer tricks on what it has got at hand.

As things are with the Arduino environment at the moment, this requires your source code to be an Arduino library as well. This isn't a big deal, there are two steps:

1. Move your project to Arduino's library folder (usually in the `Documents\Arduino\libraries` folder in your home folder on Windows and in the `Arduino/libraries` folder in your home folder on linux). More information ~here~ no longer available, link that used to work is now 404.
2. Create a `library.properties` file according to this [specification](https://arduino.github.io/arduino-cli/latest/library-specification/) in the root of your library. An example is:

```text
name=your-library
version=1.0.0
author=Your name <your email>
maintainer=Your name <your email>
sentence=Test library
paragraph=Links an archive
category=Other
url=http://example.com/
architectures=*
precompiled=true
```

The `precompiled` keyword enables us to link against some archive. The archives have to be placed in `src/<board>/`, as detailed [here](https://arduino.github.io/arduino-cli/latest/library-specification/#precompiled-binaries). Board has to for example be `avr128db64`.

After this, you should be able to compile your code against the archive.
