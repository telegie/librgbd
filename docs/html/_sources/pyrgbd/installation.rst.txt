pyrgbd installation
--------------------

=================
PyPI packages
=================

PyPI packages are available for Linux (x64) and macOS (x64/Apple Silicon) for CPython 3.6+. A package for Windows (x64) is not yet available, but is currently in testing. Stay tuned.

.. code-block::

    pip install pyrgbd

==================================
Manual Build/Installation
==================================

-------------------
Linux Requirements
-------------------

.. code-block::

    add-apt-repository ppa:deadsnakes/ppa
    apt install python3.9-dev python3.9-distutils libgl1 python3-venv python3-cachecontrol

-------------------
Setup
-------------------

.. code-block::

    git submodule update --init --recursive
    python3 bootstrap.py --rebuild
    pip install .

-------------------
Run Example
-------------------

.. code-block::
    
    # From Root directory the project
    python3 example_python/pyrgbd_decoding_encoding/pyrgbd_example.py