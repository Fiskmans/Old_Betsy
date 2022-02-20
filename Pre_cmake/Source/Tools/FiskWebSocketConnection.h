#pragma once

typedef void* PVOID;
typedef PVOID BCRYPT_ALG_HANDLE;

typedef struct ssl_st SSL;
namespace Tools
{

	void ReverseBytes(char* aData, size_t aSize);

	class FiskWebSocketConnection
	{
	public:
		void Write(const std::string& aData);

		std::string GetTextPackage();
		std::string GetBinaryPackage();

		std::string WaitForTextPackage();
		std::string WaitForBinaryPackage();

		bool HasTextPackage();
		bool HasBinaryPackage();

		void CloseConnection();

		FiskWebSocketConnection(::SSL* aConnection);
		~FiskWebSocketConnection();
	private:
		std::mutex myMutex;
		std::thread myReadThread;

		::SSL* myConnection;
		void Read();

		std::stringstream ReadBuffer;

		std::queue<std::string> myTextPacketQueue;
		std::queue<std::string> myBinaryPacketQueue;

		BCRYPT_ALG_HANDLE myAlgHandle;
		void MaskData(char* aData, size_t aSize, char* aMaskingKey);

		std::string OpcodeToString(char aCode);
		enum Flags
		{
			Fin = 0x80,
			Rsv1 = 0x40,
			Rsv2 = 0x20,
			Rsv3 = 0x10,
			OpCode = 0x0F,

			Mask = 0x80,
			PayloadLength = 0x7F
		};

		enum OpCodes
		{
			Continuation = 0x0,
			Text = 0x1,
			Binary = 0x2,
			ConnectionClose = 0x8,
			Ping = 0x9,
			Pong = 0xA
		};
		char myFlags[2];
	};
}