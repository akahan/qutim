qutim_add_plugin( emoticonssettings
	EXTENSION
	EXTENSION_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/emoticonssettings.h
	EXTENSION_CLASS Core::EmoticonsSettings,qutim_sdk_0_3::StartupModule
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	DISPLAY_NAME "Emoticons settings"
	DESCRIPTION "Default settings widget set for emoticons"
)
