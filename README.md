# Cyclon RPS C++

C++ implementation of the Cyclon Random Peer Sampling (RPS) algorithm in C++ with threads. 

> S. Voulgaris, D. Gavidia, and M. van Steen, “CYCLON: Inexpensive Membership Management for Unstructured P2P Overlays,” Journal of Network and Systems Management, vol. 13, no. 2, pp. 197–217, Jun. 2005.


## How to do the things

All of the C++ source code is in `/rps-cpp/src`. There are no external dependencies. So you can build it right away from `/rps-cpp` with:

	/rps-cpp $ make 
	/rps-cpp $ ./build/rps-cpp --help 
	The best (the only) implementation of Cyclon in C++!
	Usage:
	  rps-cpp [OPTION...]

	  [...]

### Deploying in a local Docker network

You will need Python3 and Docker (preferably with the ability to launch it using your current user).

In `/python`, you will find `run_experiment.py`, which runs several `rps-cpp` Docker containers to demonstrate that the algorithm work. You will need to build the *builder* image first (its job is to compile `rps-cpp` inside a container, such that we simply drop the resulting binary inside the same kind of Docker image to have a working `rps-cpp` container):

	# Build the builder
	/rps-cpp $ make docker-builder 
	# Can you build the Docker image?
	/rps-cpp $ make docker 
	/rps-cpp $ docker images rps-cpp-img
	REPOSITORY          TAG                 [...]           SIZE
	rps-cpp-img         latest              [...]           176MB

There are python dependencies though, which are listed in `/python/requirements.txt`. You can install them locally using e.g. [virtualenv](https://virtualenv.pypa.io/en/stable/):

	# Initializes a virtualenv in /python/env (do this only once)
	/python $ virtualenv env
	# Activate the virtualenv (do this everytime you restart your shell)
	/python $ source env/bin/activate
	# Install the deps (only once...)
	(env) /python $ pip3 install -r requirements.txt

Run the experiment using:

	(env) /python $ ./run_experiment.py 

Check some logs in another window if you want:

	$ docker logs -f d0000

You're all set.

### Testing 

We set some tests using [catch2](https://github.com/catchorg/Catch2/). To launch them, do:

	/rps-cpp $ make test 
	/rps-cpp $ ./build/rps-cpp_test 
	===============================================================================
	All tests passed (29 assertions in 10 test cases)

You can add tests alonside the code. Check any `*_test.cpp` file in `/rps-cpp/src`. (I mostly did marshaling test, the coverage is not good.)

### Details 

`/rps-cpp/src/main.cpp` is the main entry point, `/rps-cpp/src/main_test.cpp` is the testing entry point (see below--don't change it), plus following packages:

* `/rps-cpp/src/marshal` contains primitive to marshal primitive types to `std::ostream`, and to unmarshal the same types from `std::istream`;
* `/rps-cpp/src/net` provides a `TCPListener` class and a `TCPConnection` class. The latter is a `std::streambuf`, meaning you can build `std::istream` and `std::ostream` out of it:
* `/rps-cpp/src/utils` contains misc stuff;
* `/rps-cpp/src/rps` contains the RPS algorithm in. `Descriptor` and `View` are data structures and are un/marshalable using streams.


#### Dependencies

Header-only C++ dependencies are already included in `/rps-cpp/lib`:

* [cxxopts](https://github.com/jarro2783/cxxopts) (for parsing parameters)
* [catch2](https://github.com/catchorg/Catch2/) (for testing)


---

Have fun!

Adrien Luxey, June 2019
