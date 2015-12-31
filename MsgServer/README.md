# *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_*

# *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_*

# *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_*
# MsgServer
#
# This the note of this server

# It is developing now, many functions are not worked well, It's my pleasure if you contact me if you meet problems when using this server
# Please e-mail me @ zhengxiaokai@dync.cc

# This server devide four part now. MsgServer, Connector, Meeting, MsgQueue
# And the client is int https://github.com/DyncKevin/MsgServerClient.git
# MsgServer just manage the left four projects, just include them.
# Connector accept connection from Clinet and WebClient, WebClient do not upload now.
# Meeting receive msg from Connector and Process the msg
# MsgQueue receive Processed msg and send back to Connector
# Connector then dispatch the msg to connected Client and push off-line msg to disconnected Client

# This server can work well on OSX; I have not tried on Linux and Windows, later I will do it.

# On OSX, after you clone MsgServer, there are four Projects now.
# Before you run this server, you need to install Redis into you system.
# Open them and run them as Connector--MsgQueue--Meeting--Client.
# You can see the log in the Console.

# *_* Later I will merge them into One Project 

# *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_*

# *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_*

# *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_* *_*
