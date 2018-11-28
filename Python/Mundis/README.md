# Mundis

![Mundis Image](https://joshbosley.com/app/Mundis-Black-Transparent.png)

***A python3-based NoSQL server***

Mundis is a simple to use NoSQL server designed in Python3 around gherkinDB.
Currently available on any Linux server with a Python3 installation, and the gherkindb package. 

*Usage*

#### Starting Mundis

	sudo mundis.py

The first time mundis.py is launched, the setup will be started and supporting directories will be created. At this point the user will be prompted for the server's public ip, the port, and information for the server's root user.
By default, root user is treated like all other users in-regard to access.
It is advised that the root user isn't used explicitly in any projects. Its usage
should be limited to the programmed usage within Mundis.

Launching, and controlling Munids

	sudo mundis.py --core start

This will launch Mundis as a demiurge (basically a better-named daemon)

Stopping and reseting Mundis

	sudo mundis.py --core stop
    sudo mundis.py --core restart
Launching the Mundis command line interface:

	sudo mundis.py --cli

At first, Mundis will ask for the Mundis root password. Once logged in,
the creation of users and databases can be performed.

	create user <username>
    create db <database>

Once Mundis is developed further, all CLI options will be stated in the 
documentation.

### Using Mundis in a project

```python3
import connector
con = connector.munconnect("127.0.0.1", 9001, server_token ,"test", "a_basic_password", 'test_db')
con.log_response = True
con.connect()

# Queries can be done here; see Syntax section.
```

***Syntax***

See: [Syntax Documentation](https://github.com/joshbosley/Mundis/blob/master/syntax.md). 


***Requirements***


*gherkindb*

A lightweight database application based on pickleDB.

The repository for gherkindb can be found [here](https://github.com/joshbosley/gherkindb), but for Mundis it must be
installed using pip3
	
    pip3 install gherkindb

*pycrypto*

    pip3 install pycrypto



