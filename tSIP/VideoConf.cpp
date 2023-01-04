//---------------------------------------------------------------------------


#pragma hdrstop

#include "VideoConf.h"
#include <json/json.h>

//---------------------------------------------------------------------------

#pragma package(smart_init)

VideoConf::VideoConf(void):
	enabled(true)
{
}

void VideoConf::fromJson(const Json::Value &jv)
{
	jv.getBool("enabled", enabled);
}

void VideoConf::toJson(Json::Value &jv)
{
	jv["enabled"] = enabled;
}

bool VideoConf::operator==(const VideoConf &right) const
{
	return (
		enabled == right.enabled
	);
}
