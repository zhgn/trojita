/* Copyright (C) 2007 - 2010 Jan Kundrát <jkt@flaska.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or version 3 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#include "FetchMsgMetadataTask.h"
#include "KeepMailboxOpenTask.h"
#include "Model.h"
#include "MailboxTree.h"

namespace Imap {
namespace Mailbox {

FetchMsgMetadataTask::FetchMsgMetadataTask( Model *_model, const QModelIndex &_mailbox, const QList<uint> &_uids ) :
    ImapTask( _model ), mailbox(_mailbox), uids(_uids)
{
    if ( uids.isEmpty() ) {
        throw CantHappen( "FetchMsgMetadataTask called with empty message set");
    }
    conn = model->findTaskResponsibleFor( mailbox );
    conn->addDependentTask( this );
}

void FetchMsgMetadataTask::perform()
{
    parser = conn->parser;
    Q_ASSERT( parser );
    model->accessParser( parser ).activeTasks.append( this );

    Q_ASSERT( ! uids.isEmpty() );
    qSort( uids );
    Sequence seq( uids.first() );
    for ( int i = 1; i < uids.size(); ++i ) {
        seq.add( uids[i] );
    }

    // we do not want to use _onlineMessageFetch because it contains UID and FLAGS
    tag = parser->uidFetch( seq, QStringList() << QLatin1String("ENVELOPE") << QLatin1String("BODYSTRUCTURE") << QLatin1String("RFC822.SIZE") );
    model->accessParser( parser ).commandMap[ tag ] = Model::Task( Model::Task::FETCH_MESSAGE_METADATA, 0 );
    emit model->activityHappening( true );
}

bool FetchMsgMetadataTask::handleFetch( Imap::Parser* ptr, const Imap::Responses::Fetch* const resp )
{
    if ( ! mailbox.isValid() ) {
        qDebug() << "FetchMsgMetadataTask::handleFetch: mailbox disappeared";
        _completed();
        // FIXME: nice error handling
        return false;
    }
    TreeItemMailbox* mailboxPtr = dynamic_cast<TreeItemMailbox*>( static_cast<TreeItemMailbox*>( mailbox.internalPointer() ) );
    Q_ASSERT(mailboxPtr);
    model->_genericHandleFetch( mailboxPtr, resp );
    return true;
}

bool FetchMsgMetadataTask::handleStateHelper( Imap::Parser* ptr, const Imap::Responses::State* const resp )
{
    if ( resp->tag.isEmpty() )
        return false;

    if ( resp->tag == tag ) {
        IMAP_TASK_ENSURE_VALID_COMMAND( tag, Model::Task::FETCH_MESSAGE_METADATA );

        if ( resp->kind == Responses::OK ) {
            // nothing should be needed here
        } else {
            // FIXME: error handling
        }
        _completed();
        IMAP_TASK_CLEANUP_COMMAND;
        return true;
    } else {
        return false;
    }
}

}
}
