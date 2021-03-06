/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "vkontakteplugin.h"
#include <qutim/debug.h>
#include "vkontakteprotocol.h"
#include "ui/vaccountcreator.h"
#include <qutim/settingslayer.h>
#include <qutim/menucontroller.h>
#include <qutim/icon.h>
#include "vaccount.h"
#include "vproxymanager.h"

void VkontaktePlugin::init()
{
	debug() << Q_FUNC_INFO;
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Vkontakte"),
			QT_TRANSLATE_NOOP("Plugin", "Simple implementation of vkontakte, based on userapi.ru"),
			PLUGIN_VERSION(0, 0, 1, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author", "Sidorov Aleksey"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("sauron@citadelspb.com"));
// 	addAuthor(QT_TRANSLATE_NOOP("Author", "Rustam Chakin"),
// 			  QT_TRANSLATE_NOOP("Task", "Founder"),
// 			  QLatin1String(""));

	ExtensionIcon vicon = ExtensionIcon("im-vkontakte");

	addExtension(QT_TRANSLATE_NOOP("Plugin", "vkontakte"),
				 QT_TRANSLATE_NOOP("Plugin", "Simple implementation of vkontakte, based on desktop.api"),
				 new GeneralGenerator<VkontakteProtocol>(),
				 vicon
				 );
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Vkontakte account creator"),
				 QT_TRANSLATE_NOOP("Plugin", "Account creator for Vkontakte"),
				 new GeneralGenerator<VAccountCreator>(),
				 vicon);
	addExtension(QT_TRANSLATE_NOOP("Plugin", "Proxy support"),
				 QT_TRANSLATE_NOOP("Plugin", "Proxy support for Vkontakte"),
				 new GeneralGenerator<VProxyManager, NetworkProxyManager>(),
				 vicon);
}
bool VkontaktePlugin::load()
{
	return true;
}
bool VkontaktePlugin::unload()
{
	return false;
}

QUTIM_EXPORT_PLUGIN(VkontaktePlugin)

