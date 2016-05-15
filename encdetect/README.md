ENCDETECT
========

ENCDETECT is a small tool to use different libraries to determine the encoding
of a RPG Maker 2000/2003 database.

ENCDETECT is part of the EasyRPG Project.
More information is available at the project website:

https://easy-rpg.org/


Documentation
-------------

Documentation is available at the documentation wiki:

https://easy-rpg.org/wiki/


Requirements
------------

 * liblcf
 * iconv
 * ICU
 * libguess
 * enca


Daily builds
------------

Up to date binaries for assorted platforms are available at:

https://easy-rpg.org/jenkins/


Source code
-----------

ENCDETECT development is hosted by GitHub, project files are available in a
git repository.

https://github.com/EasyRPG/Tools


Building
--------

ENCDETECT uses Autotools:

    ./bootstrap (only needed if using a git checkout)
    ./configure
    make
    make install (optionally)

You may tweak build parameters and environment variables, run
`./configure --help` for reference.


License
-------

ENCDETECT is free software under the MIT license. See the file COPYING for
details.
