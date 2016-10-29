/* 
 *  	Created on: 26.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlconnectionendpoint.cpp
 *		Project: Kactus 2
 */

#include "vhdlconnectionendpoint.h"

VhdlConnectionEndPoint::VhdlConnectionEndPoint():
instanceName_(),
portName_(),
signalLeft_(-1),
signalRight_(-1),
portLeft_(-1),
portRight_(-1) {
}

VhdlConnectionEndPoint::VhdlConnectionEndPoint(const QString& instanceName, 
											   const QString& portName,
											   const QString& portLeft /*= -1*/, 
											   const QString& portRight /*= -1*/, 
											   const QString& signalLeft /*= -1*/,
											   const QString& signalRight /*= -1*/ ):
instanceName_(instanceName),
portName_(portName),
signalLeft_(signalLeft),
signalRight_(signalRight),
portLeft_(portLeft),
portRight_(portRight) {
}

VhdlConnectionEndPoint::VhdlConnectionEndPoint( const VhdlConnectionEndPoint& other ):
instanceName_(other.instanceName_),
portName_(other.portName_),
signalLeft_(other.signalLeft_),
signalRight_(other.signalRight_),
portLeft_(other.portLeft_),
portRight_(other.portRight_) {
}

VhdlConnectionEndPoint& VhdlConnectionEndPoint::operator=(
	const VhdlConnectionEndPoint& other ) {

	if (this != &other) {
		instanceName_ = other.instanceName_;
		portName_ = other.portName_;
		signalLeft_ = other.signalLeft_;
		signalRight_ = other.signalRight_;
		portLeft_ = other.portLeft_;
		portRight_ = other.portRight_;
	}
	return *this;
}

bool VhdlConnectionEndPoint::operator==( const VhdlConnectionEndPoint& other ) const {
	if (instanceName_ == other.instanceName_ &&
		portName_ == other.portName_ &&
		portLeft_ == other.portLeft_ &&
		portRight_ == other.portRight_) {
		return true;
	}
	return false;
}

bool VhdlConnectionEndPoint::operator!=( const VhdlConnectionEndPoint& other ) const {
	if (instanceName_ != other.instanceName_) 
		return true;
	else if (portName_ != other.portName_)
		return true;
	else if (portLeft_ != other.portLeft_)
		return true;
	else if (portRight_ != other.portRight_)
		return true;
	else
		return false;
}

bool VhdlConnectionEndPoint::operator<( const VhdlConnectionEndPoint& other ) const {
	if (0 == instanceName_.compare(other.instanceName_.simplified(), Qt::CaseInsensitive)) {
		if (0 == portName_.compare(other.portName_.simplified(), Qt::CaseInsensitive)) {
			if (portLeft_ == other.portLeft_) {
				return portRight_ < other.portRight_;
			}
			else {
				return portLeft_ < other.portLeft_;
			}
		}
		else {
			return portName_.localeAwareCompare(other.portName_) < 0;
		}
	}
	else {
		return instanceName_.localeAwareCompare(other.instanceName_) < 0;
	}
}

bool VhdlConnectionEndPoint::operator>( const VhdlConnectionEndPoint& other ) const {
	if (0 == instanceName_.compare(other.instanceName_.simplified(), Qt::CaseInsensitive)) {
		if (0 == portName_.compare(other.portName_.simplified(), Qt::CaseInsensitive)) {
			if (portLeft_ == other.portLeft_) {
				return portRight_ > other.portRight_;
			}
			else {
				return portLeft_ > other.portLeft_;
			}
		}
		else {
			return portName_.localeAwareCompare(other.portName_) > 0;
		}
	}
	else {
		return instanceName_.localeAwareCompare(other.instanceName_) > 0;
	}
}



VhdlConnectionEndPoint::~VhdlConnectionEndPoint() {
}

QString VhdlConnectionEndPoint::instanceName() const {
	return instanceName_;
}

QString VhdlConnectionEndPoint::portName() const {
	return portName_;
}

QString VhdlConnectionEndPoint::signalLeft() const {
	return signalLeft_;
}

QString VhdlConnectionEndPoint::signalRight() const {
	return signalRight_;	
}

QString VhdlConnectionEndPoint::portLeft() const {
	return portLeft_;
}

QString VhdlConnectionEndPoint::portRight() const {
	return portRight_;
}
