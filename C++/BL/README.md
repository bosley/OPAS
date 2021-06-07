BL is a language environment I developed as an exercise in programming.   

The BL engine was made in an attempt to simulate a computer's hardware functions.
The engine does not run on byte code, it instead interprets an assembly language (.basm, .s).
Each instruction performs an action on the engine. 
The BL engine has what would equate to: memory, and a cpu.                  
For more information on the BL engine, please see Engine/Documentation.

The language developed to make programming on the system easier is BL.
For syntax information, please see Parser/Documentation.

While stable, both BL and the BL engine are under construction. 


Please report any issues if you decide to play with it.


- July 7th 2016 Update : 

	Arrays are being coded into the project at the moment, as are string comparisons.
	Once completed the instructions for use will be included in the documentation.

	Some bugs with file/cli input/output were fixed, and string 
	comparison functionality has been added.

	Inclusion of a Library is now possible as both assembly, and high-level.
	This usage has not been added to the documentation, but can be performed via
	an out-of-function declaration at the top of the file. Example  :

		# Include Low Level Library
		[math]

		# Include High Level Library
		{menu}

	To ensure that the executable can find the Library, please ensure that the 
	library code is within the subfolder ‘Library’ of the executable’s 
	current working directory.


