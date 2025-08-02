I'm using this as a devlog to collect thoughts and my roadmap. If you're a recruiter viewing this project (thanks for checking it out!) there's not gonna be much of use here. 

**Magic Numbers**

The magic numbers in this project are genuinely awful. For some unknowable reason I had this idea in my head that if you only use a magic number in one place your code is still readable and maintainable. I have come to realize that this is obviously untrue, but as I am the only current maintainer fixing it isn't my top priority. I will commit to not adding any more magic numbers with the exception of some bit indexes because I refuse to put stuff like this:
```
const THREE = 3;
```
in my code.