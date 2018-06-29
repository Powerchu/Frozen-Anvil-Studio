/* HEADER *********************************************************************************/
/*!
\file	Commands.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _COMMANDS_H_
#define _COMMANDS_H_


class CommandInvoker
{
public:


private:


};

class AbstractCommand
{
public:
	virtual void ExecuteDo() = 0;
	virtual void ExecuteUnDo() = 0;

private:


};

class ConcreteCommand : AbstractCommand
{
public:
	void ExecuteDo() override;
	void ExecuteUnDo() override;

private:


};

class CommandReceiver
{
public:


private:


};



#endif // _COMMANDS_H_
#endif // EDITOR



