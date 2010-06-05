/* Copyright (C) 2007 Jan Kundrát <jkt@gentoo.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include <QApplication>

#include "Gui/Window.h"

#ifdef HAS_GITVERSION
extern const char* gitVersion;
#endif

int main( int argc, char** argv) {
    QApplication app( argc, argv );
    Q_INIT_RESOURCE(icons);
    QCoreApplication::setApplicationName( QString::fromAscii("trojita") );
#ifdef HAS_GITVERSION
    QCoreApplication::setApplicationVersion( QString::fromAscii( gitVersion ) );
#endif
    QCoreApplication::setOrganizationDomain( QString::fromAscii("flaska.net") );
    QCoreApplication::setOrganizationName( QString::fromAscii("flaska.net") );
    app.setWindowIcon( QIcon( QString::fromAscii(":/icons/trojita.png") ) );
    Gui::MainWindow win;
    win.show();
    return app.exec();
}
