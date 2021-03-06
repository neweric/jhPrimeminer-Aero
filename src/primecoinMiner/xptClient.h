#define XPT_DEVELOPER_FEE_MAX_ENTRIES  (8)

typedef struct  
{
	uint8 merkleRoot[32];
	uint8 prevBlockHash[32];
	uint32 version;
	uint32 nonce;
	uint32 nTime;
	uint32 nBits;
	// primecoin specific
	uint32 sieveSize;
	uint32 sieveCandidate; // index of sieveCandidate for this share
	uint8 fixedMultiplierSize;
	uint8 fixedMultiplier[201];
	uint8 chainMultiplierSize;
	uint8 chainMultiplier[201];
}xptShareToSubmit_t;
typedef struct  
  {
   uint16 devFee;
   uint8  pubKeyHash[20]; // RIPEMD160 hash of public key (retrieved from wallet address without prefix byte and without checksum)
 }xptDevFeeEntry_t;
 
typedef struct  
{
#ifdef _WIN32
	SOCKET clientSocket;
#else
    int clientSocket;
#endif
	xptPacketbuffer_t* sendBuffer; // buffer for sending data
	xptPacketbuffer_t* recvBuffer; // buffer for receiving data
	// worker info
	char username[128];
	char password[128];
	uint32 clientState;
	// recv info
	uint32 recvSize;
	uint32 recvIndex;
	uint32 opcode;
	// disconnect info
	bool disconnected;
	// work data
	uint32 workDataCounter; // timestamp of when received the last block of work data
	bool workDataValid;
	xptBlockWorkInfo_t blockWorkInfo;
	xptWorkData_t workData[128]; // size equal to max payload num
	float earnedShareValue; // this value is sent by the server with each new block that is sent
	// shares to submit
#ifdef _WIN32
	CRITICAL_SECTION cs_shareSubmit;
#else
  pthread_mutex_t cs_shareSubmit;
#endif
	simpleList_t* list_shareSubmitQueue;
	bool hasWorkData;
  // timers
   uint32 time_sendPing;
   uint64 pingSum;
   uint32 pingCount;
   uint32 payloadNum;
   // developer fee
   xptDevFeeEntry_t developerFeeEntry[XPT_DEVELOPER_FEE_MAX_ENTRIES];
   sint32 developerFeeCount; // number of developer fee entries
}xptClient_t;

// connection setup
xptClient_t* xptClient_create();
bool xptClient_connect(xptClient_t* xptClient, jsonRequestTarget_t* target, uint32 payloadNum);
void xptClient_addDeveloperFeeEntry(xptClient_t* xptClient, char* walletAddress, uint16 integerFee);
void xptClient_free(xptClient_t* xptClient);
void xptClient_forceDisconnect(xptClient_t* xptClient);
  
 // connection processing
bool xptClient_process(xptClient_t* xptClient); // needs to be called in a loop
bool xptClient_isDisconnected(xptClient_t* xptClient, char** reason);
bool xptClient_isAuthenticated(xptClient_t* xptClient);
void xptClient_foundShare(xptClient_t* xptClient, xptShareToSubmit_t* xptShareToSubmit);

// never send this directly
void xptClient_sendWorkerLogin(xptClient_t* xptClient);

// packet handlers
bool xptClient_processPacket_authResponse(xptClient_t* xptClient);
bool xptClient_processPacket_blockData1(xptClient_t* xptClient);
bool xptClient_processPacket_shareAck(xptClient_t* xptClient);
bool xptClient_processPacket_message(xptClient_t* xptClient);

bool xptClient_processPacket_ping(xptClient_t* xptClient);

// util
void xptClient_getDifficultyTargetFromCompact(uint32 nCompact, uint32* hashTarget);
extern char* minerVersionString;
