<!-- This document is based on the opensource arc 42 architecture template, https://arc42.org/ -->

Introduction and Goals
======================

Business goals of the system are not clear at the first iteration and need to be clarified.
The most closest business goal: providing hash-signature of a backed-up data for further data verification and validation.

Base on the unsureness of the final businesses goals, we can call this system as a prototype for identifying functional and system uncertainties.

Requirements Overview
---------------------

From the end users point of view, system:
* Take an input file location.
* Generates hash code or checksums (see Signature Term) of the input file chunks of data.
* Writes signatures to the output file location.

System uses console interface to minimized UI-implementation and maintenance costs.
Console support 3 input parameters:
* Input file location.
* Output file location.
* Optional size of the input file binary blocks to be used for generating signatures.

System shall use advantages if multi-CPU system and be able to work with files bigger than the amount of available memory.

Signature generating function to be used (only one for the first iteration):
* MD5
* CRC

Language support is reduced to English only.

Top quality Goals
-----------------

List of the following quality goals is not final and full, but indicates the main aspect of the design and implementation: 

**Efficiency**

System need to use advantages of multi-threading and multi-processor systems, as well server and modern desktop hardware.

**Flexibility**

Supporting efficiency of the system is not possible without tasks execution flexibility.

* Reading tasks.
    * Read speed can depends on the performance of the hard drives (HDD, SDD, virtual/network drive) and their configurations (serial, parallel or others).
* Signatures generating tasks.
    * This tasks performance mostly depends on the amount of hardware/virtual CPUs.
    * Size of  chunks and hashing algorithm can affect performance.
    * Using different software or hardware hashing algorithms.
* Writing tasks.
    * Write speed can depends on the performance of the hard drives (HDD, SDD, virtual/network drive) and their configurations (serial, parallel or others).

**Platform independence**

Base on the fuzziness of the hardware environment and operations system requirements, the target system need 
to be written on the popular cross-platform low-level language with a usage of limited amount of cross-platform 
3rd-party libraries.

**Extensibility**

Base on the unsureness of the final businesses goals, the fuzziness of the hardware environment and functional requirements
(hash function) system need to be design in a aggressively extensible way.
Together with Configurability system cam also achieve platform independence, 
but be able to implement OS or hardware-specific features and improvements.


Stakeholders
------------
The following list of stakeholders is vague, but need to be taking into account on the next iteration of requirements gathering.

| Role                    | Contact                   | Expectations              |
|-------------------------|---------------------------|---------------------------|
| Development Engineers   | Unknown                   | Known and up-to-date architectural and design documentation. |
| Test Engineers          | Unknown                   | Ability to test system end-to-end and components in isolation. |
| Integration Engineers   | Unknown                   | Clear configuration documentation and extension APIs. |
| Users                   | Unknown                   | Simple user interface, clear status information. |
-----------------------------------------------------------------------------------

Architecture Constraints and limitations
========================================

C++ 14 is choose as most suitable for supporting Quality Goals section.

https://www.boost.org/ is chosen as provider of the most popular 3rd-party open-source cross-platform libraries.

Functional pattern map-filter-reduce was descoped due to initial requirements "avoid functional programming approach".

Meta-programing approach (base template multi-threading component for each subsystem), was descoped due to initial requirements "avoid meta-programming approach".

System Scope and Context
========================

System does not directly communicates with any other subsystems, except intensive IO and CPU load.

Business Context
----------------

Unknown.

Technical Context
-----------------

The scope of the application communication is limited to:
* console input and output
* reading and writing file
* configuration file manipulation
* heavy CPU and IO usage

Solution Strategy
=================

Base on the uncertainty of the final hardware and possibility to tune it up-from the 
[Message queue](https://en.wikipedia.org/wiki/Message_queue) was selected as an counter-balance for
CPU and IO usage.

Lock-free structures are selected as a base for a Message queue implementation. 
It helps to narrow down threading synchronisation penalties. 

Project structure is inspired by 
[Canonical Project Structure](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html)
and [An Introduction to Modern CMake](https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html).

Code style is driven by [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with elements of
[C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md).

Building Block View
===================

System is slitted to 3 layers and one Utility component:
* Input date reading
* Processing data
* Output dat writing
* Utilities

| **Name**              | **Responsibility**                            |
|-----------------------|-----------------------------------------------|
| *Input Data Layer*    |  *Abstracts input data caching and validation*  |
| *Output Data Layer*   |  *Abstracts output data delivery and aggregation* |
| *Processing Layer*    |  *Responsible for generating signatures*      |
| *Utilities*           |  *Set of commonly used primitive and types*   |

Runtime View
============

:x: Detailed description is not suitable for this phase of the product development.

Deployment View
===============

:x: Detailed description is not suitable for this phase of the product development.

Cross-cutting Concepts
======================

:x: Detailed description is not suitable for this phase of the product development.

Design Decisions
================

:x: Detailed description is not suitable for this phase of the product development.

Quality Requirements
====================

Risks, Concerns and Technical Debts
=========================

:x: Detailed description is not suitable for this phase of the product development.

Glossary
========

| Term                              | Definition                        |
|-----------------------------------|-----------------------------------|
| Signature                         | Signature is an abstract term, which accumulates hash and checksum algorithms. |
-------------------------------------------------------------------------
