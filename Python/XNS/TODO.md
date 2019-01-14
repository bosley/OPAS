
Josh A. Bosley
Last updated 29-May-2017 . 0327

To Do : 

				NEED THE NODES TO BE ABLE TO TRIGGER THEIR OWN HARDWARE
				ON STARTUP SEND SELF A PROBE REQUEST AND ENSURE THAT ITSSELF
				IS A KNOWN NODE. THAT SHOULD DO THE TRICK



	Core
		Triggers started- could be better. Need to start loading from file.
			Was finally able to turn a light on because of movement in a different node

		Create means to 'sanatize' data stream input from modules when reading the data in,
		and a way to massage it back when delivered to another slot

		Can send and recieve now, just need to come up with syntax best fit for
		the Core rotator and instruction methodology.

			Functions NYD in CoreInteraction 
				executeInstruct	=>  Create rotating instruction to execute stmt
				loopInstruction =>  (Currently can loop) Needs more function options than print

		Need to ensure core rotates after CoreInteraction triggers rotate in
		non-CLI mode.

		Expand on the current devices and build small motion - trigger system etc

		Build file input method for core and begin extreme testing




	Entry

		Once the Core seems completed, write in the option to run a file:
				./entry run <file>


	Settings

		Create check for change in IP, and attempt to grab the current IP of the existing
		device adapter on file


	Configure

		After the main functionality is down and tested in core and entry, reconfigurations
		need to be done to configure.py. 

		1. Re-write to class and router structure.

		2. Create a means to generate node_settings.json (Remember to include the new 'pending' variable)

		3. Revisit skeleton files, perhaps make them json. 
			Include means to store the information required to update a devices_settings.json file withou
			obliterating it, as-to generate new device files one-by-one. Thus may just mean modifying
			./configure gen devices.

		4. Create means to generate settings / device files for a whole cluster

		5. Create a merge command that adds already developed device files into an existing XNS 
		

	Package into deployable object ie. the means to setup a new cluster from scratch

		A setup script asking things like :

			How many nodes? 
			Name for each: 
				Do you know the IPS?
				Do you know the devices?
				Which hardware pack to use
				Core file for this node? 
			
			(generate all, files and settings, then zip for each)

			When launched for first time, a check will take place ensuring all setting data is set
			and valid before launch
			

	WRITE ALL OF THE DOCUMENTATION ON EVERYTHING