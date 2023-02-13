2drp
===================

## Requirements

- C++20 capable compiler.
	- Build system is configured for Visual Studio only at this moment.
- [Boost](https://www.boost.org/).
	- Requires Boost.ProgramOptions, so a header-only install will NOT work.

## Building

1. Install Boost.
	- Set your BOOST_ROOT environment variable to the install location, or;
	- Put Boost inside `./dependencies/boost`
1. Run `./build/generate_packets.bat`.
1. Run `./build/generate_vs2022.bat`.
1. Open up the resulting `./build/2drp.sln` solution.
