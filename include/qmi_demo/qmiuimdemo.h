/*
 * qmiuimdemo.h
 *
 *  Created on: 07 сент. 2018 г.
 *      Author: Korovin
 */

#ifndef INCLUDE_QMI_DEMO_QMIUIMDEMO_H_
#define INCLUDE_QMI_DEMO_QMIUIMDEMO_H_

int get_IMSI();
int get_ICCID();
int getPinState();
int VerifyPIN(char *pin);
int GetStatusConnection();
int GetIMEI();
int setModePref(int mode);

#endif /* INCLUDE_QMI_DEMO_QMIUIMDEMO_H_ */
