#include "stdafx.h"

#pragma warning (disable:4996)

void PrintCurrentTime()
{
	auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	//std::cout << std::ctime(&currentTime);
}

void DisplayWsaGetLastError(const int& wsaErrcode)
{
	LPWSTR lpMsgBuf = nullptr;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, wsaErrcode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		lpMsgBuf, 0, NULL);
	if (nullptr == lpMsgBuf) return;
	spdlog::critical("WSAErrorCode: {}, Error Message: {}", wsaErrcode, ConvertWideStringToString(lpMsgBuf));
	LocalFree(lpMsgBuf);
}

void StartLogger()
{
	auto now = std::chrono::system_clock::now();
	auto time_point = std::chrono::system_clock::to_time_t(now);
	struct std::tm* parts = std::localtime(&time_point);

	std::stringstream filename_ss;
	filename_ss << std::put_time(parts, "logs/%m-%d-%H.%M.%S.txt");
	std::string filename = filename_ss.str();

	// 로그 파일 생성
	auto file_logger = spdlog::basic_logger_mt("file_logger", filename);
	// 콘솔 출력을 위한 로거 생성
	auto console_logger = spdlog::stdout_color_mt("console_logger");
	// 파일과 콘솔에 로그 동시에 출력하는 로거 생성
	auto combined_logger = spdlog::logger("Server Log", { console_logger->sinks().front(), file_logger->sinks().front() });
	combined_logger.info("Start Logger");
	spdlog::set_default_logger(std::make_shared<spdlog::logger>(spdlog::logger("Server Log", { console_logger->sinks().front(), file_logger->sinks().front() })));
#ifdef _DEBUG
	spdlog::set_level(spdlog::level::info);
	spdlog::flush_every(std::chrono::milliseconds(10));
#else
	spdlog::set_level(spdlog::level::info);
#endif // _DEBUG
	spdlog::flush_on(spdlog::level::info);

}

std::string ConvertWideStringToString(const wchar_t* wstr)
{
	USES_CONVERSION;
	return std::string(W2A(wstr));
	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convertWchar;
	//return convertWchar.to_bytes(wstr);
}

std::wstring ConvertStringToWideString(const char* str)
{
	USES_CONVERSION;
	return std::wstring(A2W(str));
	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convertChar;
	//return convertChar.from_bytes(str);
}
