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

#include "joingroupchatmodule.h"
#include <qutim/icon.h>
#include <qutim/menucontroller.h>
#include "joingroupchat.h"
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/servicemanager.h>
#include <qutim/groupchatmanager.h>
#include <QApplication>

namespace Core
{

static bool isSupportGroupchat()
{
	foreach (GroupChatManager *manager, GroupChatManager::allManagers()) {
		Status::Type status = manager->account()->status().type();
		if (status != Status::Offline && status != Status::Connecting)
			return true;
	}
	return false;
}

JoinGroupChatModule::JoinGroupChatModule()
{
	QObject *contactList = ServiceManager::getByName("ContactList");
	if (contactList) {
		MenuController *controller = qobject_cast<MenuController*>(contactList);
		Q_ASSERT(controller);
        m_gen.reset(new JoinGroupChatGenerator(this));
        controller->addAction(m_gen.data());
	}
}

JoinGroupChatModule::~JoinGroupChatModule()
{

}

void JoinGroupChatModule::onJoinGroupChatTriggered()
{
	if (!m_chat)
		m_chat = new JoinGroupChat(qApp->activeWindow());
	m_chat.data()->setParent(QApplication::activeWindow());
#if defined (QUTIM_MOBILE_UI)
	m_chat.data()->showMaximized();
#else
	centerizeWidget(m_chat.data());
	m_chat.data()->show();
#endif
}

JoinGroupChatGenerator::JoinGroupChatGenerator(QObject* module):
	ActionGenerator(Icon("meeting-attending"),
		QT_TRANSLATE_NOOP("JoinGroupChat", "Join groupchat"),
		module,
		SLOT(onJoinGroupChatTriggered())
		)
{

}

void JoinGroupChatGenerator::showImpl(QAction *action, QObject *)
{
	action->setEnabled(isSupportGroupchat());
}
}

