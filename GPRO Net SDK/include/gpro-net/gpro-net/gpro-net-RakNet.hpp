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

	gpro-net-RakNet.hpp
	Header for RakNet common management.
*/

#ifndef _GPRO_NET_RAKNET_HPP_
#define _GPRO_NET_RAKNET_HPP_
#ifdef __cplusplus


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"
#include "RakNet/GetTime.h"


namespace gproNet
{
	// eSettings
	//	Enumeration of common settings.
	enum eSettings
	{
		SET_GPRO_SERVER_PORT = 7777,
	};


	// eMessageCommon
	//	Enumeration of custom common message identifiers.
	enum eMessageCommon
	{
		ID_GPRO_MESSAGE_COMMON_BEGIN = ID_USER_PACKET_ENUM,
		ID_GPRO_COMMON_SPATIAL_DATA_RECEIVE,
		ID_GPRO_COMMON_SPACIAL_DATA_SEND,
		ID_GPRO_MESSAGE_COMMON_END
	};

	// Description of spatial pose.
	struct sSpatialPose
	{
		float scale[3];     // non-uniform scale
		float rotate[3];    // orientation as Euler angles
		float translate[3]; // translation

		short compScale[3]; //compressed scale
		short compRotate[3]; //compressed rotation
		short compTranslate[3]; //compressed translation

		// read from stream
		RakNet::BitStream& Read(RakNet::BitStream& bitstream)
		{
			bitstream.Read(compScale[0]);
			bitstream.Read(compScale[1]);
			bitstream.Read(compScale[2]);
			bitstream.Read(compRotate[0]);
			bitstream.Read(compRotate[1]);
			bitstream.Read(compRotate[2]);
			bitstream.Read(compTranslate[0]);
			bitstream.Read(compTranslate[1]);
			bitstream.Read(compTranslate[2]);
			return bitstream;
		}

		// write to stream
		RakNet::BitStream& Write(RakNet::BitStream& bitstream) const
		{
			bitstream.Write(compScale[0]);
			bitstream.Write(compScale[1]);
			bitstream.Write(compScale[2]);
			bitstream.Write(compRotate[0]);
			bitstream.Write(compRotate[1]);
			bitstream.Write(compRotate[2]);
			bitstream.Write(compTranslate[0]);
			bitstream.Write(compTranslate[1]);
			bitstream.Write(compTranslate[2]);
			return bitstream;
		}

		//Compresses data to send
		void CompressData(RakNet::BitStream& bitstream)
		{
			//Compression
			for (int i = 0; i < 3; i++)
			{
				//Compress scale
				short cScale = scale[i] * sqrt(2) * 511; //I believe this is what we did in class
				compScale[i] = cScale;

				//Compress rotation
				short cRot = rotate[i] * sqrt(2) * 511;
				compRotate[i] = cRot;

				//Compress Translation
				short cTranslate = translate[i] * sqrt(2) * 511;
				compTranslate[i] = cTranslate;

			}

			//Write to bitstream
			Write(bitstream);
		}

		//Decompresses received data
		void DecompressData(RakNet::BitStream& bitstream)
		{
			//Read from bitstream
			Read(bitstream);

			//Decompression
			for (int i = 0; i < 3; i++)
			{
				//Decompress scale
				float dScale = compScale[i] / (sqrt(2) * 511); // multiply by reciprocal of compression to decompress
				scale[i] = dScale;

				//Decompress rotation
				float dRot = compRotate[i] / (sqrt(2) * 511);
				rotate[i] = dRot;

				//Decompress translation
				float dTranslate = compTranslate[i] / (sqrt(2) * 511);
			}
		}
	};


	// cRakNetManager
	//	Base class for RakNet peer management.
	class cRakNetManager abstract
	{
		// protected data
	protected:
		// peer
		//	Pointer to RakNet peer instance.
		RakNet::RakPeerInterface* peer;

		// protected methods
	protected:
		// cRakNetManager
		//	Default constructor.
		cRakNetManager();

		// ~cRakNetManager
		//	Destructor.
		virtual ~cRakNetManager();

		// ProcessMessage
		//	Unpack and process packet message.
		//		param bitstream: packet data in bitstream
		//		param dtSendToReceive: locally-adjusted time difference from sender to receiver
		//		param msgID: message identifier
		//		return: was message processed
		virtual bool ProcessMessage(RakNet::BitStream& bitstream, RakNet::SystemAddress const sender, RakNet::Time const dtSendToReceive, RakNet::MessageID const msgID);

		// WriteTimestamp
		//	Write timestamp ID and current time.
		//		param bitstream: packet data in bitstream
		//		return: bitstream
		RakNet::BitStream& WriteTimestamp(RakNet::BitStream& bitstream);

		// ReadTimestamp
		//	Read timestamp ID and current time.
		//		param bitstream: packet data in bitstream
		//		return: bitstream
		RakNet::BitStream& ReadTimestamp(RakNet::BitStream& bitstream, RakNet::Time& dtSendToReceive_out, RakNet::MessageID& msgID_out);

		// WriteTest
		//	Write test greeting message.
		//		param bitstream: packet data in bitstream
		//		param message: message string
		//		return: bitstream
		RakNet::BitStream& WriteTest(RakNet::BitStream& bitstream, char const message[]);

		// ReadTest
		//	Read test greeting message.
		//		param bitstream: packet data in bitstream
		//		return: bitstream
		RakNet::BitStream& ReadTest(RakNet::BitStream& bitstream);

		// public methods
	public:
		// MessageLoop
		//	Unpack and process packets.
		//		return: number of messages processed
		int MessageLoop();
	};

}


#endif	// __cplusplus
#endif	// !_GPRO_NET_RAKNET_HPP_