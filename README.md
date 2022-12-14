# WordStats

Copyright (C) 2022 - Pablo Yaggi

A solution to the problem of finding five English words with 25 distinct characters, using different algorithms.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.

## Description

This program is inspired by the [A problem squared podcast](https://aproblemsquared.libsyn.com/) (finding five English words with 25 distinct characters) by Hill and Parker, and by the great [solution proposed by Benjamin Paassen](https://gitlab.com/bpaassen/five_clique).

I was wondering if it would be possible to improve both approaches, Parker's and Benjamin's. So I came out with two main factors to work with:

1. Make a better letter colission check.
I solved this using a signature for every word, the signature is a 32bit integer with each bit corresponding to a letter in the word. Then any kind of check is performed on those signatures and not in the string.

2. Parallelize the algorithms.
I used openmp to parellize the process.

## Results

Matt's **all check** algorithm: >30 days

Benjamin's **graph** algorithm: 22 minutes

WordStats using **all check** algorithm: 25 minutes

WordStats using **parallel all check** algorithm: 7.5 minutes

WordStats using **graph** algorithm: 6 minutes

WordStats using **parallel graph** algorithm: 107 seconds



## Compiling / Running
1. Download the `words_alpha.txt` file from https://github.com/dwyl/english-words (this is the same file that Parker and Benjamin used).
2. The project is in c++ based in CMake project manager, here is a short step by step for thouse unfamiliar with the language or cmake, in linux if you have basic development tools installed, it should be enough.
```
> git clone ...
> cd wordstats
> mkdir build
> cd build
> cmake ..
> make
> ./wordstats
```
After running wordstats without arguments you'll get the list of options.
