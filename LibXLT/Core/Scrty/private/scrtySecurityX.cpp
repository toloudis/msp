/*****************************************************************************
**  scrtySecurityX.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Core/scrty/scrtySecurityX.hpp"


//============================================================================
//	scrtyDongleDoesntExistX - security isn't attached
//============================================================================
scrtyDongleDoesntExistX::scrtyDongleDoesntExistX(){}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string scrtyDongleDoesntExistX::GetErrorMessage() const
{
	return "Security doesn't exist";
}


//============================================================================
//	scrtyAPIFailedX - security API initialization failed
//============================================================================
scrtyAPIFailedX::scrtyAPIFailedX(){}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string scrtyAPIFailedX::GetErrorMessage() const
{
	return "Security API failed";
}


//============================================================================
//	scrtyInvalidLicenseX - Invalid license
//============================================================================
scrtyInvalidLicenseX::scrtyInvalidLicenseX(){}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string scrtyInvalidLicenseX::GetErrorMessage() const
{
	return "Security license invalid";
}


//============================================================================
//	scrtyVersionIncorrectX - security API version is incorrect
//============================================================================
scrtyVersionIncorrectX::scrtyVersionIncorrectX(){}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string scrtyVersionIncorrectX::GetErrorMessage() const
{
	return "Security version incorrect";
}


//============================================================================
//	scrtyReadFailedX - security read failed
//============================================================================
scrtyReadFailedX::scrtyReadFailedX(){}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string scrtyReadFailedX::GetErrorMessage() const
{
	return "Security read failed";
}


//============================================================================
//	scrtyWriteFailedX - security Write failed
//============================================================================
scrtyWriteFailedX::scrtyWriteFailedX(){}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string scrtyWriteFailedX::GetErrorMessage() const
{
	return "Security write failed";
}



//============================================================================
//	scrtyEncryptFailedX - security Encrypt failed
//============================================================================
scrtyEncryptFailedX::scrtyEncryptFailedX(){}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string scrtyEncryptFailedX::GetErrorMessage() const
{
	return "Security encrypt failed";
}


//============================================================================
//	scrtyDecryptFailedX - security Decrypt failed
//============================================================================
scrtyDecryptFailedX::scrtyDecryptFailedX(){}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string scrtyDecryptFailedX::GetErrorMessage() const
{
	return "Security decrypt failed";
}


//============================================================================
//	scrtyInvalidUserX - security invalid user
//============================================================================
scrtyInvalidUserX::scrtyInvalidUserX(){}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string scrtyInvalidUserX::GetErrorMessage() const
{
	return "Security invalid user failed";
}


//============================================================================
//	scrtyMemSizeErrorX - security memory size error
//============================================================================
scrtyMemSizeErrorX::scrtyMemSizeErrorX(){}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string scrtyMemSizeErrorX::GetErrorMessage() const
{
	return "Security memory size failed";
}


//============================================================================
//	scrtyPortAddrNotPresentX - security port address not present
//============================================================================
scrtyPortAddrNotPresentX::scrtyPortAddrNotPresentX(){}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string scrtyPortAddrNotPresentX::GetErrorMessage() const
{
	return "Security port address not present failed";
}


//============================================================================
//	scrtySecurityCountExpiredX - security count expired
//============================================================================
scrtySecurityCountExpiredX::scrtySecurityCountExpiredX(){}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string scrtySecurityCountExpiredX::GetErrorMessage() const
{
	return "Security usage expired failed";
}


//============================================================================
//	scrtySecurityDateExpiredX - security date expired
//============================================================================
scrtySecurityDateExpiredX::scrtySecurityDateExpiredX(){}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string scrtySecurityDateExpiredX::GetErrorMessage() const
{
	return "Time usage expired failed";
}


//============================================================================
//	scrtyUnknownX - unknown exception
//============================================================================
scrtyUnknownX::scrtyUnknownX(){}

//----------------------------------------------------------------------------
// Returns error message string
//----------------------------------------------------------------------------
std::string scrtyUnknownX::GetErrorMessage() const
{
	return "Unknown security error";
}
