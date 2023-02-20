#include "QtMetaParser.h"
#include <bytes.hpp>
#include <kernwin.hpp>
#include "./Public/IDAWrapper.h"

QtMetaParser::QtMetaParser()
{

}

QtMetaParser::~QtMetaParser()
{

}

bool QtMetaParser::parseMetaData(ea_t addr)
{
	struct QtMetaDataHeader
	{
		unsigned int revision;
		unsigned int classname;
		unsigned int classinfoCount, classinfoData;
		unsigned int methodCount, methodData;
		unsigned int propertyCount, propertyData;
		unsigned int enumeratorCount, enumeratorData;
		unsigned int constructorCount, constructorData;
		unsigned int flags;
		unsigned int signalCount;
	};

	ea_t startAddr = addr;
	QtMetaDataHeader header;
	get_bytes(&header,sizeof(QtMetaDataHeader), startAddr);
	startAddr = startAddr + sizeof(QtMetaDataHeader);


	msg("start print signal(%d)\n", header.signalCount);
	for (unsigned int n = 0; n < header.signalCount; ++n) {
		unsigned int signalIdx = get_dword(startAddr);
		std::string signalName = this->stringDataList[signalIdx];
		unsigned int argCount = get_dword(startAddr + 0x4);
		unsigned int paramCount = get_dword(startAddr + 0x8);
		unsigned int tag = get_dword(startAddr + 0xC);
		unsigned int flags = get_dword(startAddr + 0x10);
		msg("%d--%s\n", signalIdx, signalName.c_str());
		startAddr = startAddr + 0x14;
	}

	msg("start print slot(%d)\n", header.methodCount - header.signalCount);
	for (unsigned int n = 0; n < header.methodCount - header.signalCount; ++n) {
		unsigned int slotIdx = get_dword(startAddr);
		std::string slotName = this->stringDataList[slotIdx];
		unsigned int argCount = get_dword(startAddr + 0x4);
		unsigned int paramCount = get_dword(startAddr + 0x8);
		unsigned int tag = get_dword(startAddr + 0xC);
		unsigned int flags = get_dword(startAddr + 0x10);
		msg("%d--%s\n", slotIdx, slotName.c_str());
		startAddr = startAddr + 0x14;
	}



	return true;
}

bool QtMetaParser::parseStringData(ea_t addr)
{
	ea_t startAddr = addr;
	//确认过眼神,是正确的数据
	while (get_dword(startAddr) == 0xFFFFFFFF && !get_dword(startAddr + 8)) {
		std::uint32_t len = get_dword(startAddr + 0x4);
		std::uint32_t offset = get_dword(startAddr + 0x10);
		std::string tmpStr;
		if (len) {
			tmpStr = IDAWrapper::get_shortstring(startAddr + offset);
		}
		stringDataList.push_back(tmpStr);
		startAddr = startAddr + 0x18;
	}
	return true;
}


void QtMetaParser::StartParse()
{
	ea_t addr = get_screen_ea();
	msg("parse addr:%08X\n",addr);
	QMetaObject_d metaObject = { 0 };
	if (get_bytes(&metaObject, sizeof(metaObject), addr) != sizeof(metaObject)) {
		int a=0;
	}
	

	parseStringData(metaObject.stringdata);
	parseMetaData(metaObject.data);
}