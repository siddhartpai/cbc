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

#include "CbobData.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "../../kernel/cbob/cbob.h"

CbobData::CbobData()
{
    int i;
    char devname[32];

    m_sensors = open("/dev/cbc/sensors", O_RDONLY);

    // motor control
    m_allPID = open("/dev/cbc/pid", O_RDONLY);
    m_allPWM = open("/dev/cbc/pwm", O_RDONLY);
    for(i=0;i<4;i++){
        sprintf(devname, "/dev/cbc/pid%d", i);
        m_pid[i] = open(devname, O_RDWR);
        sprintf(devname, "/dev/cbc/pwm%d", i);
        m_pwm[i] = open(devname, O_RDWR);
    }

    // Servo control
    for(i=0;i<4;i++){
        sprintf(devname, "/dev/cbc/servo%d",i);
        m_servo[i] = open(devname, O_RDWR);
    }
}

CbobData::~CbobData()
{
    int i;
    close(m_sensors);

    this->motorsOff();
    close(m_allPID);
    close(m_allPWM);
    for(i=0;i<4;i++) {
        close(m_pid[i]);
        close(m_pwm[i]);
    }

    this->disableServos();
    for(i=0;i<4;i++) close(m_servo[i]);
}

CbobData *CbobData::instance()
{
    static CbobData cbob_data;
    
    return &cbob_data;
}

int CbobData::analog(int port)
{
    return m_sensorData[port+1];
}
int CbobData::analogPullups()
{
  return m_sensorData[13];
}
int CbobData::digital(int port)
{
    return (m_sensorData[0]&(1<<port)) && 1;
}

int CbobData::accelerometerX()
{
    return m_sensorData[10];
}
int CbobData::accelerometerY()
{
    return m_sensorData[11];
}
int CbobData::accelerometerZ()
{
    return m_sensorData[12];
}

float CbobData::batteryVoltage()
{
    //float volts = m_cbobData.shared().volts;
    
    //volts /= 4095.0;cpp
    
    //volts *= 8.4;
   return ((float)m_sensorData[9])/1000.0; 
}

int CbobData::motorVelocity(int motor)
{
    return 0;
}
int CbobData::motorPosition(int motor)
{
    if(motor >= 0 && motor <= 3)
      return m_pidData[motor];
    return 0;
}
int CbobData::motorPWM(int motor)
{
	if(motor >= 0 && motor < 4)
		return m_pwmData[motor];
	return 0;
}
void CbobData::motorGains(int motor,int *gains)
{
  short data[6];
  int i;
  ioctl(m_pid[motor], CBOB_PID_GET_GAINS, data);
  for(i=0;i<6;i++) gains[i] = data[i];
}
void CbobData::motorSetGains(int motor,int *gains)
{
    short data[6];
    int i;
    for(i=0;i<6;i++) data[i] = gains[i];
    ioctl(m_pid[motor], CBOB_PID_SET_GAINS, data);
}
void CbobData::moveMotorPower(int motor,int power)
{
    signed char pwm = power;
    write(m_pwm[motor], &pwm, 1);
}
void CbobData::moveAtVelocity(int motor,int velocity)
{
    short v = velocity;
    write(m_pid[motor], &v, 2);
}
void CbobData::moveToPosition(int motor,int speed,int target_position)
{
        short v = speed;
        int p = target_position;
        char outdata[6];

        memcpy(outdata, &v, 2);
        memcpy(outdata+2, &p, 4);

        write(m_pid[motor], outdata, 6);
}
void CbobData::motorsOff()
{
    int i;
    for(i=0;i<4;i++) this->moveAtVelocity(i,0);
}
void CbobData::clearMotorCounter(int motor)
{
    ioctl(m_pid[motor], CBOB_PID_CLEAR_COUNTER);
}

void CbobData::disableServos()
{
    int i;
    // disable servos
    for(i=0;i<4;i++) this->setServoPosition(i,-1);
}
void CbobData::setServoPosition(int servo, int pos)
{
    short position = pos;

    write(m_servo[servo], &position, 2);
}
int CbobData::getServoPosition(int servo)
{
    short position;

    read(m_servo[servo], &position, 2);

    return position;
}

void CbobData::allStop()
{
    this->motorsOff();
    this->disableServos();
    emit eStop();
}

void CbobData::updateSensors()
{
    int error;
    error = read(m_sensors, m_sensorData, 28);
    if(error < 0) perror("Got an error reading the sensor data");
    error = read(m_allPID, m_pidData, 16);
    if(error < 0) perror("Got an error reading the motor counters");
    error = read(m_allPWM, m_pwmData, 4);
    if(error < 0) perror("Got an error reading the motor pwm values");
}


