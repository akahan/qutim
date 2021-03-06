/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Denis Daschenko <daschenko@gmail.com>
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

#ifndef JABBERSETTINGS_H
#define JABBERSETTINGS_H

#include <qutim/settingswidget.h>
#include <qutim/configbase.h>
#include <QPointer>

namespace Ui
{
class JMainSettings;
}

namespace Jabber
{
using namespace qutim_sdk_0_3;
class JAccount;
class JMainSettings: public SettingsWidget
{
	Q_OBJECT
public:
	JMainSettings();
	~JMainSettings();
	void loadImpl();
	void cancelImpl();
	void saveImpl();
	virtual void setController(QObject *controller);
private:
	Ui::JMainSettings *ui;
	QPointer<JAccount> m_account;
};
}

#endif // JABBERSETTINGS_H

