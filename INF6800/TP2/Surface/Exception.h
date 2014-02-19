#ifndef	_EXCEPTION_H_
#define _EXCEPTION_H_


#include <string>


class	Exception
{
public:
	Exception( const std::string& strMessage)	{ m_strMessage = strMessage;	}
	virtual ~Exception()						{								}

	virtual std::string	GetMessage()			{ return m_strMessage;			}

protected:
	std::string m_strMessage;
};


#endif
