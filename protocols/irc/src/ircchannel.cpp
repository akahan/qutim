/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "ircchannel_p.h"
#include "ircchannelparticipant.h"
#include "ircaccount_p.h"
#include <qutim/messagesession.h>
#include <QDateTime>

namespace qutim_sdk_0_3 {

namespace irc {

IrcJoinLeftActionGenerator::IrcJoinLeftActionGenerator(QObject *receiver, const char *member) :
	ActionGenerator(QIcon(), "", receiver, member)
{
}

void IrcJoinLeftActionGenerator::showImpl(QAction *action, QObject *obj)
{
	IrcChannel *channel = qobject_cast<IrcChannel*>(obj);
	if (!channel)
		return;
	if (channel->isJoined())
		action->setText(QT_TR_NOOP("Leave the channel"));
	else
		action->setText(QT_TR_NOOP("Join the channel"));
}

IrcChannel::IrcChannel(IrcAccount *account, const QString &name) :
	Conference(account), d(new IrcChannelPrivate)
{
	d->name = name;
	d->isJoined = false;
}

IrcChannel::~IrcChannel()
{
}

Buddy *IrcChannel::me() const
{
	return d->me.data();
}

void IrcChannel::join()
{
	join(d->lastPassword);
}

void IrcChannel::join(const QString &pass)
{
	d->lastPassword == pass;
	QString cmd;
	if (!pass.isEmpty())
		cmd = QString("JOIN %1 :%2").arg(d->name).arg(pass);
	else
		cmd = QString("JOIN %1").arg(d->name);
	account()->send(cmd);
	account()->d->groupManager->updateRecent(d->name, pass);
}

void IrcChannel::leave()
{
	leave(false);
}

void IrcChannel::leave(bool force)
{
	ChatSession *session = ChatLayer::instance()->getSession(this, false);
	if (force)
		clear(session);
	else
		account()->send(QString("PART %1").arg(d->name));

	// If the channel is not in bookmarks, delete it
	if (d->bookmarkName.isEmpty()) {
		if (session)
			// The channel window is open, wait until user would close it
			connect(session, SIGNAL(destroyed()), this, SLOT(deleteLater()));
		else
			deleteLater();
	}
}

QString IrcChannel::id() const
{
	return d->name;
}

QString IrcChannel::title() const
{
	return d->bookmarkName.isEmpty() ? d->name : d->bookmarkName;
}

bool IrcChannel::sendMessage(const Message &message)
{
	Status::Type status = account()->status().type();
	if (status == Status::Connecting || status == Status::Offline)
		return false;
	QString text = message.text();
	if (text.startsWith('/')) {
		QHash<QChar, QString> extParams;
		extParams.insert('n', d->name);
		account()->send(text.mid(1), true, IrcCommandAlias::Channel, extParams);
	} else {
		foreach (const QString &line, text.split(QLatin1Char('\n')))
			account()->send(QString("PRIVMSG %1 :%2").arg(d->name).arg(line), true);
	}
	return true;
}

QString IrcChannel::topic() const
{
	return d->topic;
}

ChatUnitList IrcChannel::lowerUnits()
{
	ChatUnitList users;
	if (d->me)
		users << d->me.data();
	foreach (const QSharedPointer<IrcChannelParticipant> &user, d->users)
		users << user.data();
	return users;
}

bool IrcChannel::isJoined() const
{
	return d->isJoined;
}

void IrcChannel::setAutoJoin(bool autojoin)
{
	if (autojoin == d->autojoin)
		return;
	d->autojoin = autojoin;
	account()->groupChatManager()->setAutoJoin(this, autojoin);
	emit autoJoinChanged(autojoin);
}

bool IrcChannel::autoJoin()
{
	return d->autojoin;
}

void IrcChannel::setBookmarkName(const QString &name)
{
	QString current = title();
	d->bookmarkName = name;
	emit titleChanged(current, title());
}

const IrcAccount *IrcChannel::account() const
{
	Q_ASSERT(qobject_cast<const IrcAccount*>(Conference::account()));
	return reinterpret_cast<const IrcAccount*>(Conference::account());
}

IrcAccount *IrcChannel::account()
{
	Q_ASSERT(qobject_cast<IrcAccount*>(Conference::account()));
	return reinterpret_cast<IrcAccount*>(Conference::account());
}

IrcChannelParticipant *IrcChannel::participant(const QString &nick)
{
	if (d->me && nick == d->me->name())
		return d->me.data();
	return d->users.value(nick).data();
}

QList<IrcChannelParticipant*> IrcChannel::participants()
{
	QList<IrcChannelParticipant*> users;
	foreach (const QSharedPointer<IrcChannelParticipant> &user, d->users)
		users << user.data();
	return users;
}

void IrcChannel::onMyNickChanged(const QString &nick)
{
	addSystemMessage(tr("You are now known as %1").arg(nick), nick);
}

void IrcChannel::onParticipantNickChanged(const QString &nick)
{
	QHash<QString, ParticipantPointer>::iterator itr = d->users.begin();
	QHash<QString, ParticipantPointer>::iterator endItr = d->users.end();
	ParticipantPointer user;
	QString oldNick;
	while (itr != endItr) {
		if (itr->data() == sender()) {
			user = *itr;
			oldNick = itr.key();
			d->users.erase(itr);
			break;
		}
		++itr;
	}
	if (!user)
		return;
	if (d->users.contains(nick))
		d->users.remove(nick);
	d->users.insert(nick, user);
	addSystemMessage(tr("%1 are now known as %2").arg(oldNick).arg(nick), nick);
}

void IrcChannel::onContactQuit(const QString &message)
{
	Q_ASSERT(qobject_cast<IrcChannelParticipant*>(sender()));
	IrcChannelParticipant *user = reinterpret_cast<IrcChannelParticipant*>(sender());
	handlePart(user->name(), message);
}

void IrcChannel::handleUserList(const QStringList &users)
{
	ChatSession *session = ChatLayer::instance()->getSession(this, true);
	QString myNick = account()->name();
	foreach (QString userNick, users) {
		static QSet<QChar> flags = QSet<QChar>() << '+' << '%' << '@';
		Q_ASSERT(!userNick.isEmpty());
		QChar flag = userNick.at(0);
		bool isFlag = flags.contains(flag);
		if (isFlag)
			userNick = userNick.mid(1);
		bool isMe = userNick == myNick;
		if (isMe && d->me) {
			if (isFlag)
				d->me->setFlag(flag);
			continue;
		}
		if (!isMe && d->users.contains(userNick)) {
			if (isFlag)
				d->users.value(userNick)->setFlag(flag);
			continue;
		}
		ParticipantPointer user = ParticipantPointer(new IrcChannelParticipant(this, userNick, QString()));
		if (isMe) {
			connect(user.data(), SIGNAL(nameChanged(QString,QString)), SLOT(onMyNickChanged(QString)));
			d->me = user;
		} else {
			connect(user.data(), SIGNAL(nameChanged(QString,QString)), SLOT(onParticipantNickChanged(QString)));
			connect(user.data(), SIGNAL(quit(QString)), SLOT(onContactQuit(QString)));
			d->users.insert(userNick, user);
		}
		if (isFlag)
			user->setFlag(flag);
		if (session)
			session->addContact(user.data());
	}
	session->activate();
}

void IrcChannel::handleJoin(const QString &nick, const QString &host)
{
	if (nick == account()->name()) { // We have been connected to the channel.
		d->isJoined = true;
		emit joined();
	} else if (!d->users.contains(nick)) { // Someone has joined the channel.
		ParticipantPointer user = ParticipantPointer(new IrcChannelParticipant(this, nick, host));
		connect(user.data(), SIGNAL(nameChanged(QString,QString)), SLOT(onParticipantNickChanged(QString)));
		connect(user.data(), SIGNAL(quit(QString)), SLOT(onContactQuit(QString)));
		d->users.insert(nick, user);
		ChatSession *session = ChatLayer::instance()->getSession(this, false);
		if (session)
			session->addContact(user.data());
		addSystemMessage(tr("%1 (%2) has joined the channel").arg(nick).arg(host), nick, Notification::ChatUserJoined);
	} else {
		debug() << nick << "already presents in" << d->name;
	}
}

void IrcChannel::handlePart(const QString &nick, const QString &leaveMessage)
{
	if (nick == account()->name()) {
		ChatSession *session = ChatLayer::instance()->getSession(this, false);
		if (!leaveMessage.isEmpty())
			addSystemMessage(tr("You left this channel (%1)").arg(leaveMessage), nick, Notification::ChatUserLeft);
		else
			addSystemMessage(tr("You left this channel"), nick, Notification::ChatUserLeft);
		clear(session);
	} else if (ParticipantPointer user = d->users.take(nick)) {
		ChatSession *session = ChatLayer::instance()->getSession(this, false);
		if (session)
			session->removeContact(user.data());
		if (!leaveMessage.isEmpty())
			addSystemMessage(tr("%1 has left this channel (%2)").arg(nick).arg(leaveMessage), nick, Notification::ChatUserLeft);
		else
			addSystemMessage(tr("%1 has left this channel").arg(nick), nick, Notification::ChatUserLeft);
	} else {
		debug() << nick << "does not present in" << d->name;
	}
}

void IrcChannel::handleKick(const QString &nick, const QString &by, const QString &leaveMessage)
{
	if (nick == account()->name()) {
		ChatSession *session = ChatLayer::instance()->getSession(this, false);
		if (!leaveMessage.isEmpty()) {
			addSystemMessage(tr("%1 has kicked you from the channel (%2)")
							 .arg(nick).arg(leaveMessage),
							 nick,
							 Notification::ChatUserLeft);
		} else {
			addSystemMessage(tr("%1 has kicked you from the channel").arg(nick),
							 nick,
							 Notification::ChatUserLeft);
		}
		clear(session);
	} else if (ParticipantPointer user = d->users.take(nick)) {
		ChatSession *session = ChatLayer::instance()->getSession(this, false);
		if (!leaveMessage.isEmpty()) {
			addSystemMessage(tr("%1 has kicked %2 (%3)")
							 .arg(by).arg(nick).arg(leaveMessage),
							 nick,
							 Notification::ChatUserLeft);
		} else {
			addSystemMessage(tr("%1 has kicked %2")
							 .arg(by).arg(nick),
							 nick,
							 Notification::ChatUserLeft);
		}
		clear(session);
	} else {
		debug() << nick << "does not present in" << d->name;
	}
}

void IrcChannel::handleTopic(const QString &topic)
{
	QString previous = d->topic;
	d->topic = topic;
	emit topicChanged(topic, previous);
	addSystemMessage(tr("The channel topic is \"%1\"").arg(topic));
}

void IrcChannel::handleTopicInfo(const QString &user, const QString &timeStr)
{
	int time_t = timeStr.toInt();
	if (!time_t)
		return;
	QDateTime time = QDateTime::fromTime_t(time_t);
	addSystemMessage(tr("The topic was set by %1 on %2.")
					 .arg(user).arg(time.toString(Qt::SystemLocaleShortDate)));
}

void IrcChannel::handleMode(const QString &who, const QString &mode, const QString &param)
{
	QChar action = mode[0];
	if (action == '+') {
		for (int i = 1; i < mode.size(); ++i)
			setMode(who, mode[i], param);
	} else if (action == '-') {
		for (int i = 1; i < mode.size(); ++i)
			removeMode(who, mode[i], param);
	} else {
		foreach (QChar m, mode)
			setMode(who, m, param);
	}
}

void IrcChannel::setMode(const QString &who, QChar mode, const QString &param)
{
	if (mode == 'o' || mode == 'h' || mode == 'v') {
		IrcChannelParticipant *user = participant(param);
		if (user) {
			user->setMode(mode);
			QString msg;
			if (mode == 'o')
				msg = QT_TRANSLATE_NOOP("IrcChannel", "%1 gives channel operator privileges to %2.");
			else if (mode == 'h')
				msg = QT_TRANSLATE_NOOP("IrcChannel", "%1 gives channel halfop privileges to %2.");
			else
				msg = QT_TRANSLATE_NOOP("IrcChannel", "%1 gives %2 the permission to talk.");
			addSystemMessage(msg.arg(who).arg(param), user->name());
		} else {
			debug() << "Unknown paricipant" << param << "on the channel" << id();
		}
	} else {
		debug() << "Unknown mode" << mode;
	}
}

void IrcChannel::removeMode(const QString &who, QChar mode, const QString &param)
{
	ChatSession *session = ChatLayer::instance()->getSession(this, false);
	if (mode == 'o' || mode == 'h' || mode == 'v') {
		IrcChannelParticipant *user = participant(param);
		if (user) {
			user->removeMode(mode);
			if (session) {
				QString msg;
				if (mode == 'o')
					msg = QT_TRANSLATE_NOOP("IrcChannel", "%1 takes channel operator privileges from %2.");
				else if (mode == 'h')
					msg = QT_TRANSLATE_NOOP("IrcChannel", "%1 takes channel halfop privileges from %2.");
				else
					msg = QT_TRANSLATE_NOOP("IrcChannel", "%1 takes the permission to talk from %2.");
				addSystemMessage(msg.arg(who).arg(param), user->name());
			}
		} else {
			debug() << "Unknown paricipant" << param << "on the channel" << id();
		}
	} else {
		debug() << "Unknown mode" << mode;
	}
}

void IrcChannel::addSystemMessage(const QString &message, const QString &sender, Notification::Type type)
{
	NotificationRequest request(type);
	request.setObject(this);
	request.setText(message);
	request.setProperty("senderName", sender);
	request.send();
}

void IrcChannel::clear(ChatSession *session)
{
	if (session)
		session->removeContact(d->me.data());
	d->me = ParticipantPointer();
	emit meChanged(0);
	foreach (const ParticipantPointer &user, d->users) {
		if (session)
			session->removeContact(user.data());
	}
	d->users.clear();
	d->isJoined = false;
	emit left();
}

}

} // namespace qutim_sdk_0_3::irc

