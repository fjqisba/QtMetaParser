#include "IDAWrapper.h"
#include <kernwin.hpp>
#include <bytes.hpp>
#include <diskio.hpp>
#include <name.hpp>
#include <typeinf.hpp>
#include <frame.hpp>

namespace IDAWrapper {


	std::string IDAWrapper::get_shortstring(ea_t addr)
	{
		if (addr <= 0){
			return "";
		}
		char buffer[255] = { 0 };
		//没读取到完整的字节应该算是错误了
		if (get_bytes(buffer, sizeof(buffer), addr, GMB_READALL, NULL) != sizeof(buffer)){
			return "";
		}
		std::string ret = buffer;
		return ret;
	}


}