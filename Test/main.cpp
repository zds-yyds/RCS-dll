#include "DBCache.h"
#include "Signature.h"
#include <iostream>
#include <windows.h>

using namespace HY;
void setConsoleUTF8() {
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
}

int main() {
	setConsoleUTF8();
	// 设置 locale 让 std::cout 能正确处理中文
#if 0
	RCSCache cache;

	if (!cache.loadFromDB("localhost", "root", "123456", "test_rcs")) {
		return 1;
	}

	auto f16_records = cache.findByAircraft("F-16");
	std::cout << "F-16一共: " << f16_records.size() << u8"个 \n" << std::endl;
#endif

#if 1
	RCSCache path;
	Signature sig(path);
	if (!sig.loadFromDB("localhost", "root", "123456", "test_rcs")) {
		return 1;
	}
	else {
		auto f16_records = sig.findByAircraft("F-16");
		std::cout << "F-16一共: " << f16_records.size() << "个 \n" << std::endl;
	}

	/*{ 119.1692, 26.7048, 8000, 110, 0, 0 }   正北  -- 0 */  
	/*{ 120.17096,25.4970,8000,110,0,0 }   heading脸上  -- 1 */
	/*{ 121.9738,24.69609,8000,110,0,0 }   heading脸上大于300KM + 10  -- 0 */
	/*{ 121.69078,24.7679,8000,110,0,0 }   heading脸上小于300KM - 10  -- 1 */

	double RCS = sig.GetRCS({119.04,25.777,10,110,0,0}, 300000, -30, 30, 0, 60, 5000/*mHz*/, polarization::HH, "F-16", { 121.69078,24.7679,8000,110,0,0 });
	std::cout << "RCS: " << RCS << " \n" << std::endl;
	double IR = sig.GetIR({ 119.04,25.777,10,110,0,0 }, 20, thrustState::AB, "F-16", { 121.69078,24.7679,8000,110,0,0 });
	std::cout << "IR: " << IR << " \n" << std::endl;
	std::vector<HY::ESMRecord> ESM = sig.GetESM({ 119.04,25.777,10,110,0,0 }, "F-16", { 121.69078,24.7679,8000,-180,0,0 });
	std::vector<HY::ESMRecord> ECM = sig.GetESM({ 119.04,25.777,10,110,0,0 }, "F-16", { 121.69078,24.7679,8000,-180,0,0 });
	return 0;

#endif

#if 0
	// 尝试显式加载 DLL
	HMODULE hModule = LoadLibraryA("HYSimulation.dll");
	if (!hModule) {
		std::cout << "无法加载 HYSimulation.dll! 错误: " << GetLastError() << std::endl;
		return 1;
	}
	std::cout << "DLL 加载成功!" << std::endl;
	/*Signature sig = (Signature)hModule;
	if (!sig.cache.loadFromDB("localhost", "root", "123456", "test_rcs")) {
		return 1;
	}
	else {
		auto f16_records = sig.cache.findByAircraft("F-16");
		std::cout << "F-16一共: " << f16_records.size() << "个 \n" << std::endl;
	}*/
	FreeLibrary(hModule);
	return 0;
#endif
}