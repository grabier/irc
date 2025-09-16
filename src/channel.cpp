# include "channel.hpp"
# include "client.hpp"

Channel::Channel(void)
:  _name(), _topic(), _key(), _client_limit(100),
_invite_only(false), _topic_restricted(false),
_has_key(false), _has_client_limit(false),
_client_list(), _operators(), _invited_clients()
{}

Channel::Channel(const Channel& other)
{
	this->_name = other._name;
	this->_topic = other._topic;
	this->_key = other._key;
	this->_client_limit = other._client_limit;
	this->_invite_only = other._invite_only;
	this->_topic_restricted = other._topic_restricted;
	this->_has_key = other._has_key;
	this->_has_client_limit = other._has_client_limit;
	this->_client_list = other._client_list;
	this->_operators = other._operators;
	this->_invited_clients = other._invited_clients;

}

Channel& Channel::operator=(const Channel& other)
{
	if (this != &other)
	{
		this->_name = other._name;
		this->_topic = other._topic;
		this->_key = other._key;
		this->_client_limit = other._client_limit;
		this->_invite_only = other._invite_only;
		this->_topic_restricted = other._topic_restricted;
		this->_has_key = other._has_key;
		this->_has_client_limit = other._has_client_limit;
		this->_client_list = other._client_list;
		this->_operators = other._operators;
		this->_invited_clients = other._invited_clients;
	}
	return (*this);
}

Channel::~Channel(void) {} // check is anything must be deleted

Channel::Channel(const std::string& name) 
:  _name(name), _topic(), _key(), _client_limit(100),
_invite_only(false), _topic_restricted(false),
_has_key(false), _has_client_limit(false),
_client_list(), _operators(), _invited_clients()
{}

bool	Channel::addClient(Client& client)
{
	if ((this->_has_client_limit && this->_client_list.size() < this->_client_limit
		|| !this->_has_client_limit))
	{
		this->_client_list.push_back(&client);
		return (true);
	}
	return (false);
}

bool	Channel::removeClient(const Client& client)
{
	for (std::list<Client*>::iterator it = this->_client_list.begin();
		it != this->_client_list.end(); it++)
	{
		if (getClientName(client) == getClientName(it)) // esto lo debe implementar Client
		{
			this->_client_list.erase(it);
			return (true);
		}
	}
	return (false);
}

bool	Channel::hasClient(const Client& client) // checks if client is un channel
{
	for (std::list<Client*>::iterator it = this->_client_list.begin();
		it != this->_client_list.end(); it++)
		if (getClientName(client) == getClientName(it)) // esto lo debe implementar Client
			return (true);
	return (false);
}

std::string	Channel::getName(void) const
{
	return (this->_name);
}

size_t	Channel::getClientCount(void) const
{
	return (this->_client_list.size());
}

bool	Channel::addOperator(Client& client)
{
	if (hasClient(client))
	{
		for (std::list<Client*>::iterator it = this->_client_list.begin();
			it != this->_client_list.end(); it++)
		{
			if (getClientName(client) == getClientName(it)) // esto lo debe implementar Client
			{
				// quizas haya que cambiar algun atributo en Client tmb -> Client::isOperator(client)
				this->_operators.push_back(*it);
				return (true);
			}
		}
	}
	else
	{
		this->_operators.push_back(&client);
		return (true);
	}
	return (false);
}

bool	Channel::removeOperator(const Client& client)
{
	for (std::list<Client*>::iterator it = this->_operators.begin();
		it != this->_operators.end(); it++)
	{
		if (getClientName(client) == getClientName(it)) // esto lo debe implementar Client
		{
			this->_operators.erase(it);
			return (true);
		}
	}
	return (false);
}

bool	Channel::removeInvitedClient(const Client& client)
{
	for (std::list<Client*>::iterator it = this->_invited_clients.begin();
		it != this->_invited_clients.end(); it++)
	{
		if (getClientName(client) == getClientName(it)) // esto lo debe implementar Client
		{
			this->_invited_clients.erase(it);
			return (true);
		}
	}
	return (false);
}

bool	Channel::isOperator(const Client& client)
{
	for (std::list<Client*>::iterator it = this->_operators.begin();
		it != this->_operators.end(); it++)
		if (getClientName(client) == getClientName(it)) // esto lo debe implementar Client
			return (true);
	return (false);
}

bool	Channel::isInvitedClient(const Client& client)
{
	for (std::list<Client*>::iterator it = this->_invited_clients.begin();
		it != this->_invited_clients.end(); it++)
		if (getClientName(client) == getClientName(it)) // esto lo debe implementar Client
			return (true);
	return (false);
}

bool	Channel::setTopic(std::string& topic)
{
	if (!this->_topic_restricted)
	{
		this->_topic = topic;
		return (true);
	}
	return (false);
}

std::string	Channel::getTopic(void)
{
	return (this->_topic);
}

bool	Channel::kickClient(const Client& client) // requires privileges, and broadcasts a message 
{
	if (isOperator(client))
		removeOperator(client);
	if (isInvitedClient(client))
		removeInvitedClient(client);
	if (removeClient(client))
		return (true);
	return (false);
}

bool	Channel::inviteClient(Client&inviter, Client& client)
{
	if (!isOperator(inviter) || hasClient(client))
		return (false);
	if (isInvitedClient(client))
		return (false);
	this->_invited_clients.push_back(&client);
	return (true);
}

bool	Channel::setMode(char mode, std::string param, Client& client)
{
	if (mode == 'i')
	{
		(void)param;
		(void)client;
		this->_invite_only = true;
		return (true);
	}
	if (mode == 't')
	{
		(void)param;
		(void)client;
		this->_topic_restricted = true;
		return (true);
	}
	if (mode == 'k')
	{
		(void)client;
		this->_has_key = true;
		this->_key = param;
		return (true);
	}
	if (mode == 'o')
	{
		(void)param;
		addOperator(client);
		return (true);
	}
	if (mode == 'l')
	{
		for (int i = 0; param[i]; i++)
			if (!isdigit(param[i]))
				return (false);
		(void)client;
		this->_has_client_limit = true;
		std::istringstream s(param);
		int number;
		s >> number;
		this->_client_limit = number;
		return (true);
	}
	return (false);
}

bool	Channel::removeMode(char mode, Client& client)
{
	if (mode == 'i')
	{
		(void)client;
		this->_invite_only = false;
		return (true);
	}
	if (mode == 't')
	{
		(void)client;
		this->_topic_restricted = false;
		return (true);
	}
	if (mode == 'k')
	{
		(void)client;
		this->_has_key = false;
		this->_key = "";
		return (true);
	}
	if (mode == 'o')
	{
		if (!isOperator(client))
			return (false);
		removeOperator(client);
		return (true);
	}
	if (mode == 'l')
	{
		(void)client;
		this->_has_client_limit = false;
	}
}

bool	Channel::isInviteOnly(void)
{
	if (this->_invite_only)
		return (true);
	return (false);
}

bool	Channel::isTopicRestricted(void)
{
	if (this->_topic_restricted)
		return (true);
	return (false);
}

bool	Channel::broadcastMessage(const std::string message)
{
	// Send message to all clients of this channel
}

bool	Channel::canJoin(const Client& client)
{
	if (this->_invite_only)
		return (false);
	if (this->_has_client_limit && this->_client_limit <= this->_client_list.size())
		return (false);
	if (this->_has_key /*&& this->_key != client.key  o similar*/)
		return (false);
	return (true);
}
