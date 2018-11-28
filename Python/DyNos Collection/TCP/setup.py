"""
DynosTCP
--------

dynostcp makes threaded TCP servers and connections to servers really easy.

```````````````

::


::

    $ pip3 install rynostcp


"""

from distutils.core import setup

setup(name = "dynostcp",
    version="0.1.0",
    description="A TCP client/server framework",
    author="Josh Bosley",
    author_email="bosley117@gmail.com",
    license="MIT",
    url="https://github.com/Dynamic-Network-Overlay-System/DyNOS-TCP",
    long_description=__doc__,
    classifiers = [
        "Programming Language :: Python",
        "License :: OSI Approved :: MIT License",
        "Intended Audience :: Developers",
        "Topic :: System :: Networking" ],
    packages=['dynostcp'])

