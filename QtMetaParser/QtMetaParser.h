#pragma once
#include "Common.h"
#include <pro.h>
#include <vector>

//每个类包含的私有d数据

struct QMetaObject_d 
{
	ea_t superdata;
	ea_t stringdata;
	ea_t data;
	ea_t static_metacall;
	ea_t relatedMetaObjects;
	//reserved for future use
	ea_t extradata;
};

struct QtMetaData 
{
	unsigned int revision;
	unsigned int classname;
	unsigned int classinfoCount, classinfoData;
	unsigned int methodCount, methodData;
	unsigned int propertyCount, propertyData;
	unsigned int enumeratorCount, enumeratorData;
	unsigned int constructorCount, constructorData;
	unsigned int signalCount;
};

struct QMethodData_4
{
	//函数签名
	std::string methodSignature;
	//参数名称
	std::string paramName;
	//返回值类型
	std::string retType;
	//完整的源码
	std::string functionSrc;
};

struct QMethodData
{
	//函数名称
	std::string methodName;
	//返回值类型
	std::string retType;
	//参数个数
	unsigned int argCount;
	//参数所在偏移
	std::uint32_t paramOffset;
	//参数类型
	std::vector<std::string> paramsType;
	//参数名
	std::vector<std::string> paramsName;
};

class QtMetaParser
{
public:
	QtMetaParser();
	~QtMetaParser();
public:
	void StartParse();
private:
	bool parseStringData(ea_t addr);
	bool parseMetaData(ea_t addr);
	bool parseMetaData_4(ea_t addr);
	std::string getParamType(std::uint32_t paramIndex);
private:
	std::vector<std::string> stringDataList;
	QMetaObject_d metaObject;
};