# EEProbe CNT files converter to CSV via LIBEEP

Do you casually happen to have files produced by [ANT Neuro](https://ant-neuro.com/about-ant) hardware? The files have
.cnt extension, and are in EEProbe/CNT format _(not to be confused with Neuroscan .cnt format)_. The files are large, e.g.
containing data samples at 1000 Hz frequency or similar.

ANT Neuro does have a C library to read them: LIBEEP ([sourceforge](https://sourceforge.net/projects/libeep/) and [ANT website](http://download.ant-neuro.com/matlab/)).

So here's a little tool that:
* Reads all .cnt files in the current folder,
* Ignores samples from beginning and end of the file that "seem invalid" (invalid samples are deemed the ones where channel unit is `%` and value
  is outside of 0..100 range, or in general where any value is outside -million..+million range).
* For the rest of the samples (i.e. valid ones), averages their values over each minute.
* Outputs channel titles and the "average over each minute" channel values as a CSV file, with the same name as input CNT file, just extension changed to `.csv`

That's it!

I only checked the build on Windows (Visual Studio 2022) and Mac (clang 14). In theory any C++17 compiler should do.

The embedded `libeep-3.3.177` source is from the [SourceForge page](https://sourceforge.net/projects/libeep/) above. License of that is GPLv3, and the original authors are: Max-Planck Institute of Cognitive Neuroscience, Germany; ANT Software BV, The Netherlands; eemagine Medical Imaging Solutions GmbH, Germany; Radboud University, Nijmegen, The Netherlands; Philipps-Universität Marburg, Germany.
