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

#ifndef ANTISPAM_HANDLER_H
#define ANTISPAM_HANDLER_H
#include <qutim/messagehandler.h>
#include <QStringList>

namespace Antispam {

class Handler : public QObject, public qutim_sdk_0_3::MessageHandler
{
	Q_OBJECT
public:
    explicit Handler();
public slots:
	void loadSettings();
protected:
	bool eventFilter(QObject *obj, QEvent *event);
    virtual Result doHandle(qutim_sdk_0_3::Message& message, QString* reason);
private:
	bool m_enabled;
	bool m_handleAuth;
	QString m_question;
	QString m_success;
	QStringList m_answers;
};

} // namespace Antispam

#endif // ANTISPAM_HANDLER_H

