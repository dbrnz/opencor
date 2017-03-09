Jupyter Notebooks
=================

Normal Operation
----------------

1) Start a notebook server: `$ jupyter notebook`

* Creates a token for verifying incoming HTTP requests.

2) Connect a web-browser to the server:

* Uses server token for authentication.
* Response sets a cookie for subsequent authentication.
* Response page shows a directory tree.
* Objects in tree are (sub-)directories and files.
* Some files are notebooks (they have an extension of `ipynb`)
* Page also has an option to create a `New` notebook.

3) User either (double-)clicks a notebook file or selects `New`:

* Response page is the notebook (a blank notebook when New).
* Javascript in the page requests the server to start a kernel for the notebook.
* The kernel's name is passed to the server.

4) Server kernel startup:

* A `kernelspec` provides kernel details (looked up by name).
* Server allocates sockets and saves details in a `connection file`.
* Server spawns kernel, passing the connection file as a parameter.
* Kernel connects to server via allocated sockets.

5) Notebook/kernel interaction.

* Javascript/AJAX code in browser communicates with server web API.
* Server passes messages between browser and kernel.


Embedded in OpenCOR
-------------------

OpenCOR *is* the kernel.

1) At JupyterKernel plugin startup time a (modified) Jupyter server process is spawned using Python's `multiprocess` module.

    * This allocates sockets (for kernel communication) and saves details in a `connection file`.
    * A kernel process isn't spawned but instead connection file details are passed back to OpenCOR (via multiprocess.Pipe).
    * Given that kernel creation is intended to be triggered by a web request (from a browser) we have to bypass this and simply go through the kernel creation process.

2)  When connection details are received OpenCOR continues with kernel startup.

3)  Authentication -- can we set a session cookie (via Qt) in OpenCOR's web browser??

    Yes -- http://doc.qt.io/qt-5/qwebenginecookiestore.html

4)  Interaction between OpenCOR's file manager and web browser.


Example Notebooks
=================

https://nbviewer.jupyter.org/github/znah/mathbox/blob/jupyter/examples/notebooks/mathbox.ipynb

https://nbviewer.jupyter.org/github/waltherg/notebooks/blob/master/2013-12-03-Crank_Nicolson.ipynb

https://github.com/unconed/mathbox
