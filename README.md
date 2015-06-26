Obsidian v3.4.1 2015-06-26
========

General
-------
Obsidian is a program designed to simplify the work of a school-book library

Libraries
---------
Obsidian needs the following libraries to operate:  
* libqt5core5
* libqt5gui5
* libqt5widgets5
* libqt5sql5
* libqt5sql5-mysql

Compiling
---------
If you want to compile Obsidian, you need `qt5-default` and the usual 
`build-essential`. To compile the source, simply invoke `make` in the `src`
directory.

Config
------
You can configure Obsidian with the file `obsidian.conf` but all config options
can also be changed in the configuration dialogue.

Database
--------
Obsidian uses MySQL to store data. To function properly, it needs a database
user with the UPDATE, DELETE, INSERT, SELECT, and EXECUTE rights. The user name
is set to `biblio` by default and the password is `Buecherverwaltungsprogramm`.
This, however, can be configured in the config dialogue. To set up the database,
you need to run the provided `buecher.sql` script with root permissions. It will
create all the necessary tables, views, and functions.

Known bugs
----------
When using a keyboard shortcut to open a dialogue, the keyboard focus doesn't
behave properly. It should be assigned to the most relevant widget (usually a
QLineEdit) and it works on my computer but apparently not on all machines.

Acknowledgements
----------------
I would like to thank Martin Hirschberger for helping me laying out the
conceptual basis for Obsidian and providing his notebook for demonstration
purposes.
