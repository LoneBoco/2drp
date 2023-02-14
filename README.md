2drp
===================

## Requirements

- C++20 capable compiler.
	- Build system is configured for Visual Studio only at this moment.
- [Git LFS](https://git-lfs.com/)
- [Boost](https://www.boost.org/).
	- Requires Boost.ProgramOptions, so a header-only install will NOT work.

## Building steps

1. Install Boost.
	- Set your BOOST_ROOT environment variable to the install location, or;
	- Put the contents of Boost inside `./dependencies/boost`
1. Make sure Git LFS is installed (check the website).
1. Clone the repository.
1. Initialize the submodules.
	- `git submodule update --init --recursive`
1. Run `./build/generate_packets.bat`.
1. Run `./build/generate_vs2022.bat`.
1. Open up the resulting `./build/2drp.sln` solution.
