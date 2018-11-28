"""
mundis
--------

Mundis description

```````````````

::

import mundis

```````````````````

::

    $ pip3 install mundis


"""

from distutils.core import setup

setup(name = "mundisdb",
    version="0.0.1",
    description="A python-based server.",
    author="Josh Bosley",
    author_email="bosley117@gmail.com",
    license="MIT",
    url="",
    long_description=__doc__,
    classifiers = [
        "Programming Language :: Python",
        "License :: OSI Approved :: MIT License",
        "Intended Audience :: Developers",
        "Topic :: Database" ],
    py_modules=['mundis'],
    install_requires=['gherkindb', 'pycrypto'])
