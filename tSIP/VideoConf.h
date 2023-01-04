//---------------------------------------------------------------------------

#ifndef VideoConfH
#define VideoConfH
//---------------------------------------------------------------------------

namespace Json
{
	class Value;
}

struct VideoConf
{
	bool enabled;

	VideoConf(void);
	void fromJson(const Json::Value &jv);
	void toJson(Json::Value &jv);

	bool operator==(const VideoConf &right) const;
	bool operator!=(const VideoConf &right) const
	{
		return !(*this == right);
	}
};


#endif
