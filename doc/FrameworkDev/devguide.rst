.. _framework-devguide-rst:

###############################
GPI Framework Developer's Guide
###############################

This guide introduces the various structures of the GPI framework, discusses
the runtime entry points and covers some of the package dependencies for
mastering a GPI distro.  Its intended for GPI developers who want to make
contributions to the framework in the areas of UI, deployment, runtime
operation, etc...  If your interested in extending the scientific algorithms
used by the framework (i.e. creating new nodes), then checkout the 
`Node Developer's Guide <http://docs.gpilab.com/en/develop/NodeDev/devguide.html>`_
as well the community page for listings of other node library projects
`<http://gpilab.com/community/>`_.

The Framework
=============

The 'framework' and 'core-node' projects are separate entities.  The framework
provides the UI, runtime environment and build environment for the node
libraries.  This means the framework doesn't provide any of the scientific
algorithms itself, however, it pulls together some basic numeric packages (C++
and Python) to facilitate the development of nodes.

The repository that holds the framework project can be accessed here:
https://github.com/gpilab/framework

The framework project is organized in the following directory structure:

- bin/
- doc/
- include/
- launch/
- lib/

The following sections introduce the software contained in each of these
directories.

bin/
----
The 'bin/' directory holds the launching mechanisms for starting GPI (either as
a GUI or a command-line utility), the command-line make, or running
the updater.  These mechanisms are accessed by the following scripts:

- gpi_launch
- gpi_make
- gpi_update

The scripts are fairly simple, they may take command-line arguments and
generally access a section of the main gpi library (discussed in the 'lib'
section).  Their purpose is to be configurable for a specific Anaconda Python
install and make OS specific changes necessary to run GPI.

For example, the following code block is take from **gpi_launch**:

.. code-block:: python

    #!/usr/bin/env python

    ...

    import sys, os

    # Check for Anaconda PREFIX, or assume that THIS file location is the CWD.
    GPI_PREFIX = '/opt/anaconda1anaconda2anaconda3' # ANACONDA
    if GPI_PREFIX == '/opt/' + 'anaconda1anaconda2anaconda3':
        GPI_PREFIX, _ = os.path.split(os.path.dirname(os.path.realpath(__file__)))
        GPI_LIB_DIR = os.path.join(GPI_PREFIX, 'lib')
        if GPI_LIB_DIR not in sys.path:
            sys.path.insert(0, GPI_LIB_DIR)

    # gpi
    from gpi import launch

    if __name__ == '__main__':
        launch.launch()

The shebang at the top specifies the first python instance in the user
environment should be used to start this launching script.  This means that
you could download the framework project and run it against any Python
installation, provided it has the necessary dependencies.  The subsequent bit
of logic determines whether the 'conda' package manager was used to install
GPI; if it has, then the **GPI_PREFIX** will point to the location of the
**gpi** library within the Anaconda Python installation.  If 'conda' wasn't
used then the script uses dead reckoning to determine the location of the
**gpi** library assuming the script was initiated within the framework
directory structure.  This is the same basic process in each script.

doc/
----
The 'doc/' directory contains the very documentation that you are reading now.
It is written in `reStructuredText <http://docutils.sourceforge.net/rst.html>`_
and is compiled using the `Sphinx <http://www.sphinx-doc.org/en/stable/index.html>`_
documentation generator.  This is auto-generated for each commit and hosted by
the `ReadTheDocs <https://readthedocs.org/>`_ project.

To build these docs locally (if you intend to modify them), install 
`Sphinx <http://www.sphinx-doc.org/en/stable/index.html>`_ and simply run:

.. code-block:: bash

    $ make html

in the 'doc/' directory.  Then open the relevant '.html' files that have been
generated under the 'doc/_build/' directory.

include/
--------
The 'include/' directory contains the API code for a Python-C interface called
**PyFI**.  While the GPI UI doesn't call on this code, it is provided as a
portability layer for GPI nodes that depend on Python-C modules, written with
:ref:`pyfi_api-rst`.  While you can still write GPI nodes with Python extension
modules supported by
`SWIG <http://www.swig.org/index.php>`_ or `Boost <http://www.boost.org/>`_
these will be extra dependencies of your node library that will have to be
communicated to your end users.

At the time PyFI was written, the aforementioned SWIG and Boost libraries
didn't yet have the capability to transfer numeric arrays between Python and C
without copying the data.  This was being developed in a project called
`Boost.NumPy <https://github.com/ndarray/Boost.NumPy>`_, and is now part of the
Boost.Python support package.  PyFI also has the capability of allocating
numeric arrays from Python, to be used in the embedded C routine, which
circumvents the need to copy data between Python and C.

You can read more about PyFI in the :ref:`Node Developer's Guide <devguide-rst>`:

- :ref:`PyFI <pyfi-devguide>`
- :ref:`pyfi_api-rst`

.. _launch-specific-details:

launch/
-------
The 'launch/' directory contains the GPI UI start-up scripts that meet the
porcelain:

- GPI.desktop
- gpi.app
- gpi.command

The **GPI.desktop** and **gpi.app** scripts are converted to icon launchers
for the Gnome (Ubuntu Linux) and MacOS/OSX desktops.  They both eventually
call on the **gpi.command** script which handles OS specific parameters for
GPI.  The launcher script's link to the **gpi.command** script is not
immediately obvious by inspecting these pieces of code, because there are path
manipulations that happen in each of these scripts when they are part of a
conda package deployment.  To see how these scripts are placed in a deployment
process, check out the conda deployment hook
`build.sh <https://github.com/gpilab/conda-distro/blob/master/gpi-framework/build.sh>`_.

As mentioned above, the **gpi.command** script provides some unique launching
parameters depending on the OS. These differences are as follows:

**In OSX**, the main OS menu-bar will display the name of the binary being run.
Since GPI is called as a library via Python, the Python binary is soft-linked
in the system's temp directory as "GPI" before calling it to start the GPI
runtime.  This will cause the OS menu-bar to display "GPI" in the upper
left-hand corner.

**In Ubuntu Linux**, there have been specific versions of the desktop
environment that cause Qt to default to one of the older style UI skins.  To
ensure that GPI is correctly started with the look and feel consistent to that
of its OSX counterpart, the "cleanlooks" style is forced as a command-line
option.

lib/
----
The 'lib/' directory contains the **'gpi'** python library.  This library is 
pure-Python and contains all the elements of the runtime environment.  The
section :ref:`gpi-lib-reference` catagorizes and lists each of the main object
classes within the **'gpi'** lib.

Building a Distro
=================

The GPI distributions leverage the Anaconda.org cloud for hosting GPI itself
and some extra binary packages that are specifically configured for use with
GPI. The supporting Anaconda-Cloud packages can be found at `anaconda.org/gpi
<https://anaconda.org/gpi>`_.  These packages were built using the scripts that
can be found in the `github.com/gpilab/conda-distro
<https://github.com/gpilab/conda-distro>`_ project.  This project has a master
build script (`build_all.py
<https://github.com/gpilab/conda-distro/blob/master/build_all.py>`_), that can
be tailored to specific package needs such as platform dependence, Python
version, release candidate, etc...

Since the gpi-framework project is pure python, it doesn't have any platform
specific building requirements.  The conda package is simply a copy of the
framework code with some specific launching details that are OS dependent (see
the :ref:`launch-specific-details` section above).

The astyle, eigen, fftw, and gpi-core-nodes packages are all C/C++ based and
require platform specific compilation.  This requires that the master build
script is run on each type of target platform, with the necessary indicator
options chosen. For example, if we want to build the framework and core-node
packages, then we'd use a command like the following:

.. code-block:: bash

    $ ./build_all.py --force-upload --auto-upload --channel gpi --tag main --python-version 35 --build-number 0 --package gpi-framework,gpi-core-nodes

This command would have to be run on both a Linux machine and an OSX machine so
that Anaconda would have each platform specific version.

To ensure a GPI compatible environment, its common to use the installer script
(`GPI_Install_Latest.sh
<https://github.com/gpilab/conda-distro/blob/master/GPI_Install_Latest.sh>`_)
to auto-install a local copy of Miniconda to run these build scripts against.
This can be done on Linux and OSX.

OSX App
-------
To create the OSX *App* distro there there is an additional project, for
generically wrapping Miniconda based applications in OSX, called
`Wr[App]-A-Conda <https://github.com/nckz/wrappaconda>`_.  The *App* can also be
bundled in a .dmg file by using the `create-dmg
<https://github.com/andreyvit/create-dmg>`_ project.

These two external application packaging projects ('Wr[App]-A-Conda' and
'create-dmg') are automatically invoked by the `build_all.sh
<https://github.com/gpilab/conda-distro/blob/master/osx_stack/build_all.sh>`_
script found in the `osx_stack
<https://github.com/gpilab/conda-distro/tree/master/osx_stack>`_ section of the
`github.com/gpilab/conda-distro <https://github.com/gpilab/conda-distro>`_
project (provided that these two projects are in your PATH environment).

.. _gpi-lib-reference:

The 'gpi' Python Library
========================

The **gpi** Python library is collection of inherited 
`PyQt <http://pyqt.sourceforge.net/Docs/PyQt4/classes.html>`_ classes (for the
UI), `Numpy <http://www.numpy.org/>`_ data handling libs, configuration,
command-line and build system interfaces.  The following sections will
introduce the sub-library components within these contexts.

GUI (PyQt Classes)
------------------
The gpi modules responsible for the canvas, node menu and other dialogues are
as follows:

.. currentmodule:: gpi.canvasGraph

- canvasGraph.py

    - .. autoclass:: GraphWidget

.. figure:: canvasFSM.png
    :align: center
    :width: 100%
    :figwidth: image

    *Figure 1* - The finite state machine model for the canvas execution.

.. currentmodule:: gpi.canvasScene

- canvasScene.py

    - .. autoclass:: CanvasScene

.. currentmodule:: gpi.mainWindow

- mainWindow.py

    - .. autoclass:: MainCanvas

.. currentmodule:: gpi.edge

- edge.py

    - .. autoclass:: Edge

    - .. autoclass:: EdgeTracer

.. currentmodule:: gpi.launch

- launch.py

    - .. autoclass:: Splash

    - .. autofunction:: launch

.. currentmodule:: gpi.layoutWindow

- layoutWindow.py

    - .. autoclass:: LayoutWindow

    - .. autoclass:: LayoutMaster

.. currentmodule:: gpi.library

- library.py

    - .. autoclass:: FauxMenu

    - .. autoclass:: NodeCatalogItem

    - .. autoclass:: NetworkCatalogItem

    - .. autoclass:: Library

.. currentmodule:: gpi.macroNode

- macroNode.py

    - .. autoclass:: EdgeNode

    - .. autoclass:: MacroAPI

    - .. autoclass:: PortEdge

    - .. autoclass:: MacroNodeEdge

    - .. autoclass:: MacroNode

.. currentmodule:: gpi.node

- node.py

    - .. autoclass:: TimerPack

    - .. autoclass:: EventManager

    - .. autoclass:: NodeSignalMediator

    - .. autoclass:: NodeAppearance

    - .. autoclass:: Node

.. figure:: nodeFSM.png
    :align: center
    :width: 100%
    :figwidth: image

    *Figure 2* - The finite state machine model for the node execution.

.. currentmodule:: gpi.nodeAPI

- nodeAPI.py

    - .. autoclass:: NodeAPI

        .. automethod:: initUI

        .. automethod:: validate

        .. automethod:: compute

.. currentmodule:: gpi.port

- port.py

    - .. autoclass:: Port

    - .. autoclass:: InPort

    - .. autoclass:: OutPort

.. currentmodule:: gpi.runnable

- runnable.py

    - .. autoclass:: Runnable

    - .. autofunction:: ExecRunnable

.. currentmodule:: gpi.update

- update.py

    - .. autoclass:: JSONStreamLoads

    - .. autoclass:: CondaUpdater

    - .. autoclass:: UpdateWindow

.. currentmodule:: gpi.widgets

- widgets.py

    - This file contains all of the Q-Widgets that have been wrapped and
      simplified for the GPI interface and Node development API.

    - .. autoclass:: GenericWidgetGroup


Node/Canvas Execution
---------------------

.. currentmodule:: gpi.functor

- functor.py

    - .. autoclass:: GPIFunctor

    - .. autoclass:: ATask

    - .. autoclass:: PTask

    - .. autoclass:: TTask

.. currentmodule:: gpi.nodeQueue

- nodeQueue.py (PyQt classes)

    - .. autoclass:: GPINodeQueue

.. currentmodule:: gpi.stateMachine

- stateMachine.py (PyQt classes)

    - .. autoclass:: GPIState

    - .. autoclass:: GPI_FSM

.. currentmodule:: gpi.topsort

- topsort.py

    - .. autofunction:: topological_sort

    - .. autofunction:: topsort

Data Types & Communication
--------------------------

Most of the data communication between nodes is handled by the 'functor'
module.  The data type descriptions are all loaded at runtime by plugging
everything under the 'types' directory.

.. currentmodule:: gpi.dataproxy

- dataproxy.py

    - .. automodule:: gpi.dataproxy

    - .. autoclass:: DataProxy

.. currentmodule:: gpi.GLObjects

- GLObjects.py

    - .. automodule:: gpi.GLObjects

.. currentmodule:: gpi.types.globjectlist_GPITYPE

- globjectlist_GPITYPE.py

    - .. autoclass:: GLOList

.. currentmodule:: gpi.types.numpy_GPITYPE

- numpy_GPITYPE.py

    - .. autoclass:: NPYarray

.. currentmodule:: gpi.types.python_GPITYPE

- python_GPITYPE.py

    - .. autoclass:: INT

    - .. autoclass:: FLOAT

    - .. autoclass:: LONG

    - .. autoclass:: COMPLEX

    - .. autoclass:: STRING

    - .. autoclass:: LIST

    - .. autoclass:: TUPLE

    - .. autoclass:: DICT

.. currentmodule:: gpi.defaultTypes

- defaultTypes.py

    - .. autoclass:: GPIDefaultType

Loading & Storing Node/Types/Widgets
------------------------------------

.. currentmodule:: gpi.loader

- loader.py

    .. automodule:: gpi.loader

    - .. autofunction:: consolidatePaths

    - .. autofunction:: PKGroot

    - .. autofunction:: loadMod

    - .. autofunction:: findAndLoadMod

.. currentmodule:: gpi.catalog

- catalog.py

    - .. autoclass:: CatalogObj

    - .. autoclass:: Catalog

    - Also see the 'library' module for more information on storing these types.

Loading & Storing Networks
--------------------------

.. currentmodule:: gpi.network

- network.py

    - .. automodule:: gpi.network

    - .. autoclass:: Network_Base

    - .. autoclass:: Network_v1

    - .. autoclass:: Network_v2

    - .. autoclass:: Network_v3

    - .. autoclass:: Network

Compiling PyFI Modules
----------------------

.. currentmodule:: gpi.make

- make.py

    - .. automodule:: gpi.make

    - .. autofunction:: make

Command Line Interface
----------------------

.. currentmodule:: gpi.cmd

- cmd.py

    - .. autoclass:: CmdParser

User Configuration
------------------

.. currentmodule:: gpi.config

- config.py

    - .. autoclass:: ConfigManager

Templates
---------

.. currentmodule:: gpi.nodeTemplate_GPI

- nodeTemplate_GPI.py

    - .. autoclass:: ExternalNode


BORG: Building Outside Relationships with GPI
---------------------------------------------

The interface for easily encapsulating/assimilating external command-line
programs is internally called "external binary encapsulation" or 'ebe'.

.. currentmodule:: gpi.ebe

- ebe.py

    - .. autoclass:: FilePath

    - .. autoclass:: Command

Misc Features
-------------

.. currentmodule:: gpi.sysspecs

- sysspecs.py

    - .. automodule:: gpi.sysspecs

.. currentmodule:: gpi.logger

- logger.py

    - .. automodule:: gpi.logger

.. currentmodule:: gpi.associate

- associate.py

    - .. automodule:: gpi.associate

    - .. autoclass:: gpi.BindCatalogItem

.. currentmodule:: gpi.defines

- defines.py

    - .. automodule:: gpi.defines

.. currentmodule:: gpi.console

- console.py (PyQt classes)

    - .. autoclass:: Tee

.. currentmodule:: gpi.syntax

- syntax.py

    - .. automodule:: gpi.syntax

.. currentmodule:: gpi.docs

- docs.py

    - .. autoclass:: NodeDocs

    - .. autoclass:: GPIdocs


