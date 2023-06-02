Getting Started
==================================
Installation from PyPI
----------------------------------
.. code-block::

    pip install pyrgbd

Supported Python Versions:

* 3.7
* 3.8
* 3.9
* 3.10

Supported Operating Systems:

* Mac
* Linux

Installation from Source
----------------------------------

.. code-block::

    git clone --recursive https://github.com/telegie/librgbd.git
    cd librgbd
    python boostrap.py
    pip install -e .

Our git repository includes submodules. Thus, the --recursive flag is required for fully cloning our repository.

While we mainly rely on CMake for build, there are libraries that we rely, but were built before CMake has become popular.
bootstrap.py is mainly for building those libraries, whose source has been downloaded as submodules in the previous step.

The final step builds pyrgbd from source using pip.

Supported Operating Systems:

* Windows
* Mac
* Linux

Check Installation
----------------------------------
Upon successful installation, following should run without error.

.. code-block::

    python -c "import pyrgbd"
