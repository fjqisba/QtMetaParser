#include "QtMetaParser.h"
#include <bytes.hpp>
#include <kernwin.hpp>
#include <map>
#include <algorithm>
#include "./Public/IDAWrapper.h"
#include "Common.h"

std::map<unsigned int, std::string> gMapQMetaType;

void InitQMetaTypeMap()
{
	gMapQMetaType[0] = "UnknownType";
	gMapQMetaType[1] = "Bool";
	gMapQMetaType[2] = "Int";
	gMapQMetaType[3] = "UInt";
	gMapQMetaType[4] = "LongLong";
	gMapQMetaType[5] = "ULongLong";
	gMapQMetaType[6] = "Double";
	gMapQMetaType[7] = "QChar";
	gMapQMetaType[8] = "QVariantMap";
	gMapQMetaType[9] = "QVariantList";
	gMapQMetaType[10] = "QString";
	gMapQMetaType[11] = "QStringList";
	gMapQMetaType[12] = "QByteArray";
	gMapQMetaType[13] = "QBitArray";
	gMapQMetaType[14] = "QDate";
	gMapQMetaType[15] = "QTime";
	gMapQMetaType[16] = "QDateTime";
	gMapQMetaType[17] = "QUrl";
	gMapQMetaType[18] = "QLocale";
	gMapQMetaType[19] = "QRect";
	gMapQMetaType[20] = "QRectF";
	gMapQMetaType[21] = "QSize";
	gMapQMetaType[22] = "QSizeF";
	gMapQMetaType[23] = "QLine";
	gMapQMetaType[24] = "QLineF";
	gMapQMetaType[25] = "QPoint";
	gMapQMetaType[26] = "QPointF";
	gMapQMetaType[27] = "QRegExp";
	gMapQMetaType[28] = "QVariantHash";
	gMapQMetaType[29] = "QEasingCurve";
	gMapQMetaType[30] = "QUuid";
	gMapQMetaType[31] = "VoidStar";
	gMapQMetaType[32] = "Long";
	gMapQMetaType[33] = "Short";
	gMapQMetaType[34] = "Char";
	gMapQMetaType[35] = "ULong";
	gMapQMetaType[36] = "UShort";
	gMapQMetaType[37] = "UChar";
	gMapQMetaType[38] = "Float";
	gMapQMetaType[39] = "QObjectStar";
	gMapQMetaType[40] = "SChar";
	gMapQMetaType[41] = "QVariant";
	gMapQMetaType[42] = "QModelIndex";
	gMapQMetaType[43] = "Void";
	gMapQMetaType[44] = "QRegularExpression";
	gMapQMetaType[45] = "QJsonValue";
	gMapQMetaType[46] = "QJsonObject";
	gMapQMetaType[47] = "QJsonArray";
	gMapQMetaType[48] = "QJsonDocument";
	gMapQMetaType[49] = "QByteArrayList";
	gMapQMetaType[50] = "QPersistentModelIndex";
	gMapQMetaType[51] = "Nullptr";
	gMapQMetaType[52] = "QCborSimpleType";
	gMapQMetaType[53] = "QCborValue";
	gMapQMetaType[54] = "QCborArray";
	gMapQMetaType[55] = "QCborMap";

	gMapQMetaType[64] = "QFont";
	gMapQMetaType[65] = "QPixmap";
	gMapQMetaType[66] = "QBrush";
	gMapQMetaType[67] = "QColor";
	gMapQMetaType[68] = "QPalette";
	gMapQMetaType[69] = "QIcon";
	gMapQMetaType[70] = "QImage";
	gMapQMetaType[71] = "QPolygon";
	gMapQMetaType[72] = "QRegion";
	gMapQMetaType[73] = "QBitmap";
	gMapQMetaType[74] = "QCursor";
	gMapQMetaType[75] = "QKeySequence";
	gMapQMetaType[76] = "QPen";
	gMapQMetaType[77] = "QTextLength";
	gMapQMetaType[78] = "QTextFormat";
	gMapQMetaType[79] = "QMatrix";
	gMapQMetaType[80] = "QTransform";
	gMapQMetaType[81] = "QMatrix4x4";
	gMapQMetaType[82] = "QVector2D";
	gMapQMetaType[83] = "QVector3D";
	gMapQMetaType[84] = "QVector4D";
	gMapQMetaType[85] = "QQuaternion";
	gMapQMetaType[86] = "QPolygonF";
	gMapQMetaType[87] = "QColorSpace";

	gMapQMetaType[121] = "QSizePolicy";
	gMapQMetaType[1024] = "User";
}

std::string QtMetaParser::getParamType(std::uint32_t paramIndex)
{
	if (paramIndex & 0x80000000) {
		return this->stringDataList[paramIndex & 0xFFFFFFF];
	}

	std::map<unsigned int, std::string>::iterator it = gMapQMetaType.find(paramIndex);
	if (it != gMapQMetaType.end()) {
		return it->second;
	}
	return "Unknown";
}

QtMetaParser::QtMetaParser()
{
	InitQMetaTypeMap();
}

QtMetaParser::~QtMetaParser()
{

}

//定制,用户识别参数名称列表
std::vector<std::string> recognizeFunctionParamsNameList(std::string paramNameSignature)
{
	std::vector<std::string> retList;
	size_t paramStart = paramNameSignature.find(',');
	while (paramStart != -1) {
		std::string tmpParamName = paramNameSignature.substr(0,paramStart);
		retList.push_back(tmpParamName);
		paramNameSignature = paramNameSignature.substr(paramStart+1);
		paramStart = paramNameSignature.find(',');
	}	
	retList.push_back(paramNameSignature);
	return retList;
}

//定制,用于识别函数签名,返回参数类型
std::vector<std::string> recognizeFunctionParamsTypeList(std::string& functionSignature)
{
	std::vector<std::string> retList;

	size_t paramStart = functionSignature.find('(');
	size_t paramEnd = functionSignature.rfind(')');
	if (paramStart == -1 || paramEnd == -1) {
		return retList;
	}
	std::string paramListString = functionSignature.substr(paramStart + 1, paramEnd - paramStart - 1);
	std::string currentParam;
	size_t openBrackets = 0;
	for (char c : paramListString) {
		if (c == '<') {
			++openBrackets;
			currentParam += c;
		}
		else if (c == '>') {
			--openBrackets;
			currentParam += c;
		}
		else if (c == ',' && openBrackets == 0) {
			retList.push_back(currentParam);
			currentParam.clear();
		}
		else {
			currentParam += c;
		}
	}
	retList.push_back(currentParam);
	return retList;
}

bool QtMetaParser::parseMetaData_4(ea_t addr)
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
	get_bytes(&header, sizeof(QtMetaDataHeader), startAddr);
	startAddr = startAddr + sizeof(QtMetaDataHeader);

	std::vector<QMethodData_4> signalMethodList;
	std::vector<QMethodData_4> slotMethodList;

	for (unsigned int n = 0; n < header.signalCount; ++n) {

		QMethodData_4 tmpMethod;
		unsigned int signalOffset = get_dword(startAddr);
		//函数签名,包含函数名称和函数类型
		tmpMethod.methodSignature = IDAWrapper::get_shortstring(metaObject.stringdata + signalOffset);
		//只有参数的名称
		tmpMethod.paramName = IDAWrapper::get_shortstring(metaObject.stringdata + get_dword(startAddr + 0x4));
		tmpMethod.retType = IDAWrapper::get_shortstring(metaObject.stringdata + get_dword(startAddr + 0x8));
		unsigned int tag = get_dword(startAddr + 0xC);
		unsigned int flags = get_dword(startAddr + 0x10);
		signalMethodList.push_back(tmpMethod);
		startAddr = startAddr + 0x14;
	}

	for (unsigned int n = 0; n < header.methodCount - header.signalCount; ++n) {
		QMethodData_4 tmpMethod;
		unsigned int slotOffset = get_dword(startAddr);
		//函数签名,包含函数名称和函数类型
		tmpMethod.methodSignature = IDAWrapper::get_shortstring(metaObject.stringdata + slotOffset);
		//只有参数的名称
		tmpMethod.paramName = IDAWrapper::get_shortstring(metaObject.stringdata + get_dword(startAddr + 0x4));
		tmpMethod.retType = IDAWrapper::get_shortstring(metaObject.stringdata + get_dword(startAddr + 0x8));
		unsigned int tag = get_dword(startAddr + 0xC);
		unsigned int flags = get_dword(startAddr + 0x10);
		slotMethodList.push_back(tmpMethod);
		startAddr = startAddr + 0x14;
	}

	//开始生成完整的函数签名
	std::string className = IDAWrapper::get_shortstring(metaObject.stringdata);
	for (unsigned int n = 0; n < signalMethodList.size(); ++n) {
		if (signalMethodList[n].retType == "") {
			signalMethodList[n].retType = "void";
		}
		std::string funcSrc = signalMethodList[n].retType + " ";
		int start = signalMethodList[n].methodSignature.find('(');
		int end = signalMethodList[n].methodSignature.find(')');
		if (start == -1 || end == -1) {
			return false;
		}
		std::string functionName = className + "::" + signalMethodList[n].methodSignature.substr(0, start);
		funcSrc = funcSrc + functionName + "(";
		//参数个数不为0
		if (end != start + 1) {
			//参数个数为1
			if (!std::count(signalMethodList[n].methodSignature.begin(), signalMethodList[n].methodSignature.end(), ',')) {
				funcSrc = funcSrc + signalMethodList[n].methodSignature.substr(start + 1, end - start - 1) + " " + signalMethodList[n].paramName;
			}
			else {
				std::vector<std::string> pararmNameList = recognizeFunctionParamsNameList(signalMethodList[n].paramName);
				std::vector<std::string> paramTypeList = recognizeFunctionParamsTypeList(signalMethodList[n].methodSignature);
				if (paramTypeList.size() != pararmNameList.size()) {
					//可能是哥写的代码出问题了
					return false;
				}
				for (unsigned int k = 0; k < paramTypeList.size(); ++k) {
					funcSrc = funcSrc + paramTypeList[k] + " " + pararmNameList[k] + ",";
				}
				funcSrc.pop_back();
			}
		}
		funcSrc = funcSrc + ")";
		signalMethodList[n].functionSrc = funcSrc;
	}

	for (unsigned int n = 0; n < slotMethodList.size(); ++n) {
		if (slotMethodList[n].retType == "") {
			slotMethodList[n].retType = "void";
		}
		std::string funcSrc = slotMethodList[n].retType + " ";
		int start = slotMethodList[n].methodSignature.find('(');
		int end = slotMethodList[n].methodSignature.find(')');
		if (start == -1 || end == -1) {
			return false;
		}
		std::string functionName = className + "::" + slotMethodList[n].methodSignature.substr(0, start);
		funcSrc = funcSrc + functionName + "(";
		//参数个数不为0
		if (end != start + 1) {
			//参数个数为1
			if (!std::count(slotMethodList[n].methodSignature.begin(), slotMethodList[n].methodSignature.end(), ',')) {
				funcSrc = funcSrc + slotMethodList[n].methodSignature.substr(start + 1, end - start - 1) + " " + slotMethodList[n].paramName;
			}
			else {
				std::vector<std::string> pararmNameList = recognizeFunctionParamsNameList(slotMethodList[n].paramName);
				std::vector<std::string> paramTypeList = recognizeFunctionParamsTypeList(slotMethodList[n].methodSignature);
				if (paramTypeList.size() != pararmNameList.size()) {
					//可能是哥写的代码出问题了
					return false;
				}
				for (unsigned int k = 0; k < paramTypeList.size(); ++k) {
					funcSrc = funcSrc + paramTypeList[k] + " " + pararmNameList[k] + ",";
				}
				funcSrc.pop_back();
			}
		}
		funcSrc = funcSrc + ")";
		slotMethodList[n].functionSrc = funcSrc;
	}

	//————开始输出结果——————
	msg_clear();
	int index = 0;
	msg("signal count(%d):\n", signalMethodList.size());
	for (unsigned int n = 0; n < signalMethodList.size(); ++n) {
		msg("%d %s\n", index++, signalMethodList[n].functionSrc.c_str());
	}
	msg("slot count(%d):\n", slotMethodList.size());
	for (unsigned int n = 0; n < slotMethodList.size(); ++n) {
		msg("%d %s\n", index++, slotMethodList[n].functionSrc.c_str());
	}
	return true;
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


	std::vector<QMethodData> signalMethodList;
	std::vector<QMethodData> slotMethodList;

	for (unsigned int n = 0; n < header.signalCount; ++n) {
		QMethodData tmpMethod;
		unsigned int signalIdx = get_dword(startAddr);
		tmpMethod.methodName = this->stringDataList[signalIdx];
		tmpMethod.argCount = get_dword(startAddr + 0x4);
		tmpMethod.paramOffset = get_dword(startAddr + 0x8);
		unsigned int tag = get_dword(startAddr + 0xC);
		unsigned int flags = get_dword(startAddr + 0x10);
		signalMethodList.push_back(tmpMethod);
		startAddr = startAddr + 0x14;
	}

	for (unsigned int n = 0; n < header.methodCount - header.signalCount; ++n) {
		QMethodData tmpMethod;
		unsigned int slotIdx = get_dword(startAddr);
		tmpMethod.methodName = this->stringDataList[slotIdx];
		tmpMethod.argCount = get_dword(startAddr + 0x4);
		tmpMethod.paramOffset = get_dword(startAddr + 0x8);
		unsigned int tag = get_dword(startAddr + 0xC);
		unsigned int flags = get_dword(startAddr + 0x10);
		slotMethodList.push_back(tmpMethod);
		startAddr = startAddr + 0x14;
	}

	for (unsigned int n = 0; n < signalMethodList.size(); ++n) {
		ea_t paramAddr = addr + (signalMethodList[n].paramOffset * 4);
		//第一个一定是返回值
		signalMethodList[n].retType = getParamType(get_dword(paramAddr));
		paramAddr = paramAddr + 4;
		//解析参数类型
		for (unsigned int m = 0; m < signalMethodList[n].argCount; ++m) {
			signalMethodList[n].paramsType.push_back(getParamType(get_dword(paramAddr)));
			paramAddr = paramAddr + 4;
		}
		//解析参数名
		for (unsigned int m = 0; m < signalMethodList[n].argCount; ++m) {
			signalMethodList[n].paramsName.push_back(stringDataList[get_dword(paramAddr)]);
			paramAddr = paramAddr + 4;
		}
	}

	for (unsigned int n = 0; n < slotMethodList.size(); ++n) {
		ea_t paramAddr = addr + (slotMethodList[n].paramOffset * 4);
		//第一个一定是返回值
		slotMethodList[n].retType = getParamType(get_dword(paramAddr));
		paramAddr = paramAddr + 4;
		//解析参数类型
		for (unsigned int m = 0; m < slotMethodList[n].argCount; ++m) {
			slotMethodList[n].paramsType.push_back(getParamType(get_dword(paramAddr)));
			paramAddr = paramAddr + 4;
		}
		//解析参数名
		for (unsigned int m = 0; m < slotMethodList[n].argCount; ++m) {
			slotMethodList[n].paramsName.push_back(stringDataList[get_dword(paramAddr)]);
			paramAddr = paramAddr + 4;
		}
	}

	//开始输出结果
	msg_clear();
	int index = 0;
	msg("signal count(%d):\n", signalMethodList.size());
	for (unsigned int n = 0; n < signalMethodList.size(); ++n) {
		std::string methodMsg = signalMethodList[n].retType + " " + signalMethodList[n].methodName + "(";
		for (unsigned int m = 0; m < signalMethodList[n].argCount; ++m) {
			methodMsg = methodMsg + signalMethodList[n].paramsType[m] + " " + signalMethodList[n].paramsName[m] + ",";
		}
		if (signalMethodList[n].argCount) {
			methodMsg.pop_back();
		}
		methodMsg = methodMsg + ")";
		msg("%d %s\n", index++, methodMsg.c_str());
	}

	msg("slot count(%d):\n", slotMethodList.size());
	for (unsigned int n = 0; n < slotMethodList.size(); ++n) {
		std::string methodMsg = slotMethodList[n].retType + " " + slotMethodList[n].methodName + "(";
		for (unsigned int m = 0; m < slotMethodList[n].argCount; ++m) {
			methodMsg = methodMsg + slotMethodList[n].paramsType[m] + " " + slotMethodList[n].paramsName[m] + ",";
		}
		if (slotMethodList[n].argCount) {
			methodMsg.pop_back();
		}
		methodMsg = methodMsg + ")";
		msg("%d %s\n", index++, methodMsg.c_str());
	}
	return true;
}

bool QtMetaParser::parseStringData(ea_t addr)
{
	ea_t startAddr = addr;
	struct stringElement
	{
		std::uint32_t startFlag;
		std::uint32_t len;
		ea_t unknown;
		ea_t offset;
	};
	while (true) {
		stringElement tmpElement = { 0 };
		if (get_bytes(&tmpElement, sizeof(stringElement), startAddr) == -1) {
			return false;
		}
		if (tmpElement.startFlag != -1 || tmpElement.unknown) {
			break;
		}
		std::string tmpStr;
		if (tmpElement.len) {
			tmpStr = IDAWrapper::get_shortstring(startAddr + tmpElement.offset);
		}
		stringDataList.push_back(tmpStr);
		startAddr = startAddr + sizeof(stringElement);
	};
	return true;
}



void QtMetaParser::StartParse()
{
	std::vector<std::string> aa = recognizeFunctionParamsNameList(",");

	ea_t addr = get_screen_ea();
	msg("parse addr:%08X\n",addr);
	metaObject = { 0 };
	if (get_bytes(&metaObject, sizeof(metaObject), addr) != sizeof(metaObject)) {
		int a = 0;
	}

	//获取QT版本号
	int revision = get_dword(metaObject.data);
	
	if (revision == 5) {
		//Qt4.x低版本
		parseMetaData_4(metaObject.data);
	}
	//To do..也许有更多分支
	else {
		parseStringData(metaObject.stringdata);
		parseMetaData(metaObject.data);
	}
}