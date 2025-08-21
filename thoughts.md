I'm using this as a devlog to collect thoughts and my roadmap. If you're a recruiter viewing this project (thanks for checking it out!) there's not gonna be much of use here. 

**Magic Numbers**

The magic numbers in this project are genuinely awful. For some unknowable reason I had this idea in my head that if you only use a magic number in one place your code is still readable and maintainable. I have come to realize that this is obviously untrue, but as I am the only current maintainer fixing it isn't my top priority. I will commit to not adding any more magic numbers with the exception of some bit indexes because I refuse to put stuff like this:
```
const THREE = 3;
```
in my code.

**STOP**

This instruction is actually not used in a single official GB rom, but it's good to have in case some crazy gameboy homebrew dev is using it in their games

**CACHE IS KING**

When writing systems code, Cache. Is. King. I think beginner systems developers like myself don't fully understand just how expensive main memory accesses are. Operating Systems classes frame memory as RAM: fast and Disc: slow. In reality, you can get the accurate square root of a 32 bit integer more than 4 times over the course of a single cache miss and subsequent main memory access. 

As an aside, if you're interested in finding out more, there's a great talk from cppcon 2014 about data-oriented programming where the speaker gives this example, among others.

An example of cache-oriented programming from this project is in the renderer. The first stage of rendering in the gameboy is a scam of OAM to find all objects that are positioned where the PPU is currently "looking". My initial implementation of this was to return a vector of addresses, as I believed it saved time by avoiding unecessary copying. However, the time wasted by copying 4 bytes of data into a struct and pushing it into a vector is actually significantly less than the time saved by avoiding cache misses when reading those addresses. 