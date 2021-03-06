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
#ifndef CHATAPPEARANCE_H
#define CHATAPPEARANCE_H

#include <QWidget>
#include <qutim/settingswidget.h>
#include <QHash>

namespace Ui
{
	class chatAppearance;
}
class QWebPage;
namespace Core
{

namespace AdiumChat
{

using namespace qutim_sdk_0_3;
class ChatVariable;
class ChatViewController;
class ChatStyleOutput; //temporary
class ChatAppearance : public SettingsWidget
{
	Q_OBJECT
public:
	ChatAppearance();
	virtual void cancelImpl();
	virtual void loadImpl();
	virtual void saveImpl();
	virtual ~ChatAppearance();
private slots:
	void onThemeChanged(int index);
	void onVariantChanged(QString variant);
	void onVariableChanged();
protected:
	void makeSettings();
private:
	void getThemes();
	void makePage();
	Ui::chatAppearance *ui;
	QHash<QString, QString> m_themes;
	QString m_current_style_name;
	QString m_current_variant;
	QList<ChatVariable *> m_current_variables;
	ChatStyleOutput *m_controller;
	bool isLoad;
	QWidget *settingsWidget;
};

}
}

#endif // CHATAPPEARANCE_H

