/**************************************************************************
 *  Copyright 2008,2009 KISS Institute for Practical Robotics             *
 *                                                                        *
 *  This file is part of CBC Firmware.                                    *
 *                                                                        *
 *  CBC Firmware is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  CBC Firmware is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this copy of CBC Firmware.  Check the LICENSE file         *
 *  in the project root.  If not, see <http://www.gnu.org/licenses/>.     *
 **************************************************************************/

#ifndef __ACCELEROMETER_H__
#define __ACCELEROMETER_H__

#include "ui_Accelerometer.h"
#include "Page.h"
#include "CbobData.h"

#include <QTimer>


class Accelerometer : public Page, private Ui::Accelerometer
{
    Q_OBJECT

public:
    Accelerometer(QWidget *parent = 0);
    ~Accelerometer();

public slots:
    void show();
    void hide();
    void updateAccel();

protected:
    void paintEvent(QPaintEvent *event);

private:
    CbobData *m_cbobData;
    QTimer m_timer;
    int scan_index;
    QPoint pointsX[100];
    QPoint pointsY[100];
    QPoint pointsZ[100];
};

#endif
