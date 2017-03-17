0_Production - Highest Tier on our software heirarchy.  Once code has reached production, it does not get changed unless absolutely necessary.

1_Testing - Once you have written your code, migrate it from 2_Development.  Once your code has passed inspection, and has been verified from at least two sources (Peter, Dustin, John), it will be returned with any improvements, bugs, etc.  Fix, develop, send for inspection, and repeat as necessary. After final inspection, the code will be migrated to 0_Production

2_Development - Put all new .ino projects and code you are working on in here.


NAMING CONVENTION:  <Project_Group>_<Project_Title>_<Initials>

Example:
	Project Group: CAN
	Project Title: Network Testing
	Initials:      DD (Dustin Dobransky)

Folder Name: CAN_NetworkTesting_DD

NOTE: All libraries (.h, .cpp files) MUST be placed in the libraries folder.  The name of the folder must be the same as the name of the files inside.
