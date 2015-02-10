# Mouse-Encoder
David and Jose's work on creating a program to pull and interpret data from the optical gaming mouse for the casting robot.

The basic idea behind this approach is simple: a program, written in Python, pulls and processes data from an optical mouse.
This data is then sent off to a client program in MATLAB which updates a requisite constant in the XPC-Target loop.

Notes:
1) Simple Python-MATLAB Socket Communication program is located in 'src/prototypes'
2) Proposed model for the system is located in 'documentation'

This software carries no guarantees, implicit or otherwise.
