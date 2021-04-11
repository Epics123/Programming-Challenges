/*
   Copyright 2021 Daniel S. Buckstein

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	GPRO Net SDK: Networking framework.
	By Daniel S. Buckstein

	gpro-net-RakNet-Server.hpp
	Header for RakNet server management.
*/

#ifndef _GPRO_NET_RAKNET_SERVER_HPP_
#define _GPRO_NET_RAKNET_SERVER_HPP_
#ifdef __cplusplus


#include "gpro-net/gpro-net/gpro-net-RakNet.hpp"

#include <vector>


namespace gproNet
{
	// eMessageServer
	//	Enumeration of custom server message identifiers.
	enum eMessageServer
	{
		ID_GPRO_MESSAGE_SERVER_BEGIN = ID_GPRO_MESSAGE_COMMON_END,
		ID_GPRO_CLIENT_MASTER_CONNECT,
		ID_GPRO_CLIENT_SERVER_CONNECT,
		ID_GPRO_CLIENT_SERVER_DISCONNECT,

		ID_GPRO_MESSAGE_SERVER_END
	};

	const int MAX_SERVERS = 4; //Arbitrary # of servers

	//Need:
	// - List of servers


	// cRakNetServer
	//	RakNet peer management for server.
	class cRakNetServer : public cRakNetManager
	{
		// public methods
	public:
		// cRakNetServer
		//	Default constructor.
		cRakNetServer();

		// ~cRakNetServer
		//	Destructor.
		virtual ~cRakNetServer();

		//Initialize
		//Starts up and initializes this server
		virtual void Initialize();

		// protected methods
	protected:
		// ProcessMessage
		//	Unpack and process packet message.
		//		param bitstream: packet data in bitstream
		//		param dtSendToReceive: locally-adjusted time difference from sender to receiver
		//		param msgID: message identifier
		//		return: was message processed
		virtual bool ProcessMessage(RakNet::BitStream& bitstream, RakNet::SystemAddress const sender, RakNet::Time const dtSendToReceive, RakNet::MessageID const msgID);
	
		sSpatialPose& GetPose();

	protected:
		sSpatialPose pose;
	};

	class cMasterServer : public cRakNetServer
	{
	public: 
		cMasterServer()
		{
			//Populate master with available servers
			for (int i = 0; i < MAX_SERVERS; i++)
			{
				//Create and initialize a new server
				cRakNetServer server = cRakNetServer();
				server.Initialize();
				availbleServers.push_back(server); //Push server to master's list
			}
		}

	protected:
		const RakNet::SystemAddress masterSystemAddress; //Address info for the master server
		std::vector<cRakNetServer> availbleServers;

		//ConnectClientToAvailableServer
		//Finds an available server to connect client to
		//		return: was client connected to a server
		bool ConnectClientToAvailableServer(); 

		// ProcessMessage
		//	Unpack and process packet message.
		//		param bitstream: packet data in bitstream
		//		param dtSendToReceive: locally-adjusted time difference from sender to receiver
		//		param msgID: message identifier
		//		return: was message processed
		virtual bool ProcessMessage(RakNet::BitStream& bitstream, RakNet::SystemAddress const sender, RakNet::Time const dtSendToReceive, RakNet::MessageID const msgID);
	};


}


#endif	// __cplusplus
#endif	// !_GPRO_NET_RAKNET_SERVER_HPP_