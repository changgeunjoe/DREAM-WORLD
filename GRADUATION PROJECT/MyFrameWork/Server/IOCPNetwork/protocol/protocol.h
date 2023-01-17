constexpr int CHAT_SIZE = 100;

// Packet ID
constexpr unsigned char CS_TEST_CHAT = 1;
constexpr unsigned char SC_TEST_CHAT = 2;


#pragma pack (push, 1)
struct CS_TEST_CHAT_PACKET {
	unsigned char size;
	char	type;
	char	message[CHAT_SIZE];
};

struct SC_TEST_CHAT_PACKET {
	unsigned char size;
	char	type;
	int		id;
	char	message[CHAT_SIZE];
};

#pragma pack (pop)