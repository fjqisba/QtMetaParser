#include <hexrays.hpp>
#include <auto.hpp>
#include <diskio.hpp>
#include <entry.hpp>
#include "QtMetaParser.h"

// Hex-Rays API pointer
hexdsp_t* hexdsp = nullptr;

//--------------------------------------------------------------------------
struct plugin_ctx_t : public plugmod_t
{
	bool inited = true;

	~plugin_ctx_t()
	{
		if (inited)
			term_hexrays_plugin();
	}
	virtual bool idaapi run(size_t) override;
};

//--------------------------------------------------------------------------
static plugmod_t* idaapi init()
{
	if (!init_hexrays_plugin()) {
		return nullptr; // no decompiler
	}
	const char* hxver = get_hexrays_version();
	msg("[QtMetaParser] plugin 1.0 loaded,Author: fjqisba\n");
	return new plugin_ctx_t;
}

//--------------------------------------------------------------------------
bool idaapi plugin_ctx_t::run(size_t)
{
	if (!auto_is_ok() && ask_yn(0, "The autoanalysis has not finished yet.\nDo you want to continue?") < 1)
	{
		return true;
	}
	if (inf_is_64bit()) {
		QtMetaParser<std::uint64_t> metaParser;
		metaParser.StartParse();
	}
	else {
		QtMetaParser<std::uint32_t> metaParser;
		metaParser.StartParse();
	}

	return true;
}

//--------------------------------------------------------------------------
static char comment[] = "It's a tool used to help Analysis Qt";

//--------------------------------------------------------------------------
//
//      PLUGIN DESCRIPTION BLOCK
//
//--------------------------------------------------------------------------

plugin_t PLUGIN =
{
  IDP_INTERFACE_VERSION,
  PLUGIN_MULTI,         // The plugin can work with multiple idbs in parallel
  init,                 // initialize
  nullptr,
  nullptr,
  comment,              // long comment about the plugin
  nullptr,              // multiline help about the plugin
  "QtMetaParser",       // the preferred short name of the plugin
  nullptr,              // the preferred hotkey to run the plugin
};