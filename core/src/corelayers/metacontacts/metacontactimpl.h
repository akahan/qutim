/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef METACONTACTIMPL_H
#define METACONTACTIMPL_H

#include <qutim/metacontact.h>

namespace Core
{
namespace MetaContacts
{
class MetaContactImpl : public qutim_sdk_0_3::MetaContact
{
	Q_OBJECT
public:
	MetaContactImpl(const QString &id);
	~MetaContactImpl();
	virtual QString id() const;

	virtual QString avatar() const;
	virtual QString name() const { return m_name; }
	virtual void setName(const QString &name);
	virtual qutim_sdk_0_3::Status status() const;
	virtual QStringList tags() const { return m_tags; }
	virtual void setTags(const QStringList &tags);
	virtual bool sendMessage(const qutim_sdk_0_3::Message &message);
	virtual void addContact(Contact *contact);
	void addContacts(QList<Contact*> contacts);
	virtual void removeContact(Contact *contact);
	virtual qutim_sdk_0_3::ChatUnitList lowerUnits();
	inline const QList<Contact*> &contacts() const { return m_contacts; }
	virtual const qutim_sdk_0_3::ChatUnit* getHistoryUnit() const;
	void setContactName(const QString &name);
	void setContactAvatar(const QString &name);
	void setContactTags(const QStringList &tags);
public slots:
	void setAvatar(const QString &path);
protected:
	void resetName();
	void resetStatus();
	void addContact(Contact* contact, bool update);
protected slots:
	void onContactStatusChanged();
private:
	virtual bool event(QEvent *ev);
	QString m_id;
	QString m_name;
	qutim_sdk_0_3::Status m_status;
	QStringList m_tags;
	QList<Contact*> m_contacts;
	QString m_lastAvatar;
};
}
}

#endif // METACONTACTIMPL_H

